#include "engine/RunModel.hpp"
#include <QDebug>

namespace QtDocxTemplate { namespace engine {

static bool isWT(const pugi::xml_node &n) { return std::strcmp(n.name(), "w:t")==0; }
static bool isWR(const pugi::xml_node &n) { return std::strcmp(n.name(), "w:r")==0; }

void RunModel::build(pugi::xml_node w_p) {
	m_text.clear();
	m_spans.clear();
	if(!w_p) return;
	// Collect descendant w:r/w:t in document order
	for(pugi::xml_node r = w_p.first_child(); r; r = r.next_sibling()) {
		if(!isWR(r)) continue;
		for(pugi::xml_node child = r.first_child(); child; child = child.next_sibling()) {
			if(isWT(child)) {
				QString t = QString::fromUtf8(child.text().get());
				Span span; span.t = child; span.r = r; span.start = m_text.size(); span.len = t.size();
				m_spans.push_back(span);
				m_text += t;
			}
		}
	}
}

pugi::xml_node RunModel::styleSourceRun(int start, int /*end*/) const {
	for(const auto &s : m_spans) {
		if(start < s.start + s.len && start >= s.start) return s.r;
	}
	return pugi::xml_node();
}

pugi::xml_node RunModel::cloneRunShallow(pugi::xml_node r) {
	if(!r) return {};
	pugi::xml_node parent = r.parent();
	pugi::xml_node newR = parent.insert_child_before(r.type(), r); // placeholder insertion before then move if needed
	newR.set_name("w:r");
	// copy rPr
	if(auto rPr = r.child("w:rPr")) {
		pugi::xml_node newPr = newR.append_child("w:rPr");
		for(auto c = rPr.first_child(); c; c = c.next_sibling()) {
			newPr.append_copy(c);
		}
	}
	return newR;
}

pugi::xml_node RunModel::makeTextRun(pugi::xml_node w_p, pugi::xml_node styleR, const QString &text, bool preserveSpace) {
	pugi::xml_node newR = w_p.insert_child_before(pugi::node_element, styleR); // will adjust position by caller
	newR.set_name("w:r");
	// copy style
	if(styleR) {
		if(auto rPr = styleR.child("w:rPr")) {
			pugi::xml_node newPr = newR.append_child("w:rPr");
			for(auto c = rPr.first_child(); c; c = c.next_sibling()) newPr.append_copy(c);
		}
	}
	pugi::xml_node t = newR.append_child("w:t");
	if(preserveSpace) t.append_attribute("xml:space") = "preserve";
	t.text().set(text.toUtf8().constData());
	return newR;
}

void RunModel::replaceRange(int start, int end,
							std::function<std::vector<pugi::xml_node>(pugi::xml_node w_p, pugi::xml_node styleR)> makeRuns) {
	if(start >= end) return;
	// Identify affected spans
	std::vector<int> indices;
	for(size_t i=0;i<m_spans.size();++i) {
		const auto &s = m_spans[i];
		if(s.start + s.len <= start) continue;
		if(s.start >= end) break;
		indices.push_back((int)i);
	}
	if(indices.empty()) return;
	pugi::xml_node w_p = m_spans[indices.front()].r.parent();
	pugi::xml_node firstRun = m_spans[indices.front()].r;
	pugi::xml_node styleR = styleSourceRun(start,end);

	// Split first and last span if partial
	auto splitT = [](pugi::xml_node tNode, int offset, bool leftPart){
		QString full = QString::fromUtf8(tNode.text().get());
		QString left = full.left(offset);
		QString right = full.mid(offset);
		if(leftPart) { // keep left in existing, create right after
			tNode.text().set(left.toUtf8().constData());
			pugi::xml_node newR = tNode.parent().parent().insert_child_after(pugi::node_element, tNode.parent());
			newR.set_name("w:r");
			if(auto rPr = tNode.parent().child("w:rPr")) { pugi::xml_node pr = newR.append_child("w:rPr"); for(auto c=rPr.first_child(); c; c=c.next_sibling()) pr.append_copy(c);}            
			pugi::xml_node newT = newR.append_child("w:t"); newT.text().set(right.toUtf8().constData());
			return newT; // new tail part
		} else { // keep right, create left before
			tNode.text().set(right.toUtf8().constData());
			pugi::xml_node newR = tNode.parent().parent().insert_child_before(pugi::node_element, tNode.parent());
			newR.set_name("w:r");
			if(auto rPr = tNode.parent().child("w:rPr")) { pugi::xml_node pr = newR.append_child("w:rPr"); for(auto c=rPr.first_child(); c; c=c.next_sibling()) pr.append_copy(c);}            
			pugi::xml_node newT = newR.append_child("w:t"); newT.text().set(left.toUtf8().constData());
			return tNode; // original now represents right side
		}
	};

	// Adjust first span if start is inside
	{
		auto &s = m_spans[indices.front()];
		if(start > s.start && start < s.start + s.len) {
			int offset = start - s.start;
			// split: left keep, right becomes new <w:t> after
			pugi::xml_node rightT = splitT(s.t, offset, true);
			// Update original span length
			s.len = offset;
			// Insert new span for right part (after s)
			Span newSpan{rightT, rightT.parent(), s.start + offset, QString::fromUtf8(rightT.text().get()).size()};
			m_spans.insert(m_spans.begin() + indices.front() + 1, newSpan);
			// Update indices (shift following by +1)
			for(auto &idx : indices) if(idx > indices.front()) ++idx;
		}
	}
	// Adjust last span if end is inside
	{
		auto &s = m_spans[indices.back()];
		if(end > s.start && end < s.start + s.len) {
			int offset = end - s.start; // keep right side -> split at offset
			// We want to keep right side, so split with leftPart=false
			splitT(s.t, offset, false);
			// Right remains in original s; so shift its start and len
			QString rightText = QString::fromUtf8(s.t.text().get());
			s.start = end; // new start
			s.len = rightText.size();
			// Elements before 's' remain.
		}
	}

	// Recompute spans to remove those fully inside [start,end)
	// We'll gather runs/t nodes to remove.
	std::vector<pugi::xml_node> runsToRemove;
	for(auto &sp : m_spans) {
		if(sp.start >= start && sp.start + sp.len <= end) {
			runsToRemove.push_back(sp.r);
		}
	}
	// Deduplicate runs
	std::vector<pugi::xml_node> uniqueRuns;
	for(auto r : runsToRemove) {
		bool seen=false; for(auto ur : uniqueRuns) if(ur == r) { seen=true; break; }
		if(!seen) uniqueRuns.push_back(r);
	}
	pugi::xml_node insertBefore = uniqueRuns.empty()? firstRun : uniqueRuns.front();
	auto newRuns = makeRuns(w_p, styleR);
	for(auto r : newRuns) {
		w_p.insert_copy_before(r, insertBefore);
	}
	for(auto r : uniqueRuns) w_p.remove_child(r);
	// NOTE: We do not rebuild spans here (not needed after replacement for this phase).
}

}} // namespace QtDocxTemplate::engine
