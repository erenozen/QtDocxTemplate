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
		if(start < s.start + s.len && start >= s.start) return s.r; // first overlapped
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
	if(!w_p) return {};
	pugi::xml_node newR = w_p.append_child("w:r");
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

void RunModel::replaceRangeStructural(int start, int end,
								std::function<std::vector<pugi::xml_node>(pugi::xml_node w_p, pugi::xml_node styleR)> makeRuns) {
	if(start >= end) return; if(start < 0) start = 0; if(end > m_text.size()) end = m_text.size(); if(start >= end) return;
	int firstIdx=-1, lastIdx=-1; for(int i=0;i<(int)m_spans.size();++i){ const auto &sp=m_spans[i]; if(sp.start + sp.len <= start) continue; if(sp.start >= end) break; if(firstIdx==-1) firstIdx=i; lastIdx=i; }
	if(firstIdx==-1) return; auto firstSpan = m_spans[firstIdx]; auto lastSpan = m_spans[lastIdx]; bool single = (firstSpan.t == lastSpan.t);
	pugi::xml_node w_p = firstSpan.r.parent(); pugi::xml_node styleR = styleSourceRun(start,end); if(!styleR) styleR = firstSpan.r;
	int firstOff = start - firstSpan.start; int lastOff = end - lastSpan.start;
	if(single) {
		QString full = QString::fromUtf8(firstSpan.t.text().get()); QString left = full.left(firstOff); QString right = full.mid(lastOff);
		pugi::xml_node anchor = firstSpan.r; // we'll remove
		// insert left segment before anchor if any
		if(!left.isEmpty()) {
			pugi::xml_node l = RunModel::makeTextRun(w_p, styleR, left, true);
			w_p.insert_copy_before(l, anchor); w_p.remove_child(l);
		}
		// insert new runs
		auto newRuns = makeRuns(w_p, styleR);
		for(auto r : newRuns) { if(!r) continue; w_p.insert_copy_before(r, anchor); w_p.remove_child(r); }
		// insert right segment
		if(!right.isEmpty()) {
			pugi::xml_node rSeg = RunModel::makeTextRun(w_p, styleR, right, true);
			w_p.insert_copy_before(rSeg, anchor); w_p.remove_child(rSeg);
		}
		if(anchor.parent()) anchor.parent().remove_child(anchor);
		return;
	}
	// multi span: collapse to single anchor after building left+new+right then remove covered runs
	QString firstText = QString::fromUtf8(firstSpan.t.text().get()); QString lastText = QString::fromUtf8(lastSpan.t.text().get());
	QString leftKeep = firstText.left(firstOff); QString rightKeep = lastText.mid(lastOff);
	pugi::xml_node anchor = firstSpan.r;
	if(!leftKeep.isEmpty()) { auto l=RunModel::makeTextRun(w_p, styleR, leftKeep, true); w_p.insert_copy_before(l, anchor); w_p.remove_child(l);} 
	auto newRuns = makeRuns(w_p, styleR); for(auto r : newRuns){ if(r){ w_p.insert_copy_before(r, anchor); w_p.remove_child(r);} }
	if(!rightKeep.isEmpty()) { auto rr=RunModel::makeTextRun(w_p, styleR, rightKeep, true); w_p.insert_copy_before(rr, anchor); w_p.remove_child(rr);} 
	// remove covered runs
	std::vector<pugi::xml_node> toRemove; for(pugi::xml_node r=firstSpan.r; r; r=r.next_sibling()){ toRemove.push_back(r); if(r==lastSpan.r) break; }
	for(auto r : toRemove) if(r.parent()) r.parent().remove_child(r);
}

void RunModel::replaceRange(int start, int end,
							std::function<std::vector<pugi::xml_node>(pugi::xml_node w_p, pugi::xml_node styleR)> makeRuns) {
	if(start >= end) return;
	// Clamp to valid range
	int total = m_text.size();
	if(start < 0) start = 0; if(end > total) end = total; if(start >= end) return;
	// Find first and last intersecting spans
	int firstIdx=-1, lastIdx=-1;
	for(int i=0;i<(int)m_spans.size();++i){ const auto &sp=m_spans[i]; if(sp.start + sp.len <= start) continue; if(sp.start >= end) break; if(firstIdx==-1) firstIdx=i; lastIdx=i; }
	if(firstIdx==-1) return;
	Span firstSpan = m_spans[firstIdx];
	Span lastSpan  = m_spans[lastIdx];
	pugi::xml_node w_p = firstSpan.r.parent();
	pugi::xml_node styleR = styleSourceRun(start,end); if(!styleR) styleR = firstSpan.r;
	int firstOff = start - firstSpan.start;
	int lastOff  = end   - lastSpan.start;
	bool single = (firstSpan.t == lastSpan.t);

	auto hasPreserve = [](pugi::xml_node t){ auto a=t.attribute("xml:space"); return a && std::strcmp(a.value(),"preserve")==0; };
	auto makeRun = [&](const QString &text){ if(text.isEmpty()) return pugi::xml_node(); return makeTextRun(w_p, styleR, text, true); };

	if(single) {
		QString full = QString::fromUtf8(firstSpan.t.text().get());
		QString left = full.left(firstOff);
		QString right = full.mid(lastOff);
		// Build replacement text including preserved outer spaces
		QString replacementAggregate;
		replacementAggregate += left;
		QString placeholderOriginal = full.mid(firstOff, end-start);
		// Generate replacement runs to capture styled replacement text
		std::vector<pugi::xml_node> replRuns = makeRuns(w_p, styleR);
		QString replacementValue; // concatenate their text
		for(auto r : replRuns) {
			if(!r) continue; for(pugi::xml_node t = r.child("w:t"); t; t = t.next_sibling("w:t")) replacementValue += QString::fromUtf8(t.text().get());
		}
		replacementAggregate += replacementValue;
		replacementAggregate += right;
		// Clean up temporary runs
		for(auto r : replRuns) if(r.parent()) r.parent().remove_child(r);
		// Overwrite original run text with aggregate and ensure xml:space="preserve"
		firstSpan.t.text().set(replacementAggregate.toUtf8().constData());
		if(!firstSpan.t.attribute("xml:space")) firstSpan.t.append_attribute("xml:space") = "preserve"; else firstSpan.t.attribute("xml:space").set_value("preserve");
		return;
	}
	// Multi-span
	QString firstText = QString::fromUtf8(firstSpan.t.text().get());
	QString lastText  = QString::fromUtf8(lastSpan.t.text().get());
	QString leftPart  = firstText.left(firstOff);
	QString rightPart = lastText.mid(lastOff);
	// Insert boundary and replacement runs before firstSpan.r (anchor)
	pugi::xml_node anchor = firstSpan.r;
	// Collect replacement value by creating runs then concatenating their text
	std::vector<pugi::xml_node> replRuns = makeRuns(w_p, styleR);
	QString replacementValue; for(auto r : replRuns) { if(!r) continue; for(pugi::xml_node t = r.child("w:t"); t; t = t.next_sibling("w:t")) replacementValue += QString::fromUtf8(t.text().get()); }
	for(auto r : replRuns) if(r.parent()) r.parent().remove_child(r);
	// Build aggregate: leftPart + replacement + rightPart
	QString aggregate = leftPart + replacementValue + rightPart;
	// Remove all covered runs and replace with a single styled run
	std::vector<pugi::xml_node> toRemove; for(pugi::xml_node r = firstSpan.r; r; r = r.next_sibling()) { toRemove.push_back(r); if(r==lastSpan.r) break; }
	pugi::xml_node styleSource = styleR;
	for(auto r : toRemove) { if(r==firstSpan.r) continue; if(r.parent()) r.parent().remove_child(r); }
	// Reuse first run's node as container
	if(firstSpan.t) {
		firstSpan.t.text().set(aggregate.toUtf8().constData());
		if(!firstSpan.t.attribute("xml:space")) firstSpan.t.append_attribute("xml:space") = "preserve"; else firstSpan.t.attribute("xml:space").set_value("preserve");
		// Ensure style of first run only (remove other style children if they exist? Already removed other runs)
	}
	return;
}

}} // namespace QtDocxTemplate::engine
