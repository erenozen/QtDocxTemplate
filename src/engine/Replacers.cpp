#include "engine/Replacers.hpp"
#include "engine/RunModel.hpp"
#include "QtDocxTemplate/Variables.hpp"
#include "QtDocxTemplate/TextVariable.hpp"
#include <QRegularExpression>
#include <unordered_map>

namespace QtDocxTemplate { namespace engine {

namespace { QString wrapKey(const QString &k,const QString &pre,const QString &suf){ return pre + k + suf; } }

void Replacers::replaceText(pugi::xml_document &doc,
							const QString &prefix,
							const QString &suffix,
							const ::QtDocxTemplate::Variables &vars) {
	// Build lookup of wrapped placeholders => replacement
	std::unordered_map<QString, QString> map; map.reserve(vars.all().size()*2);
	for(const auto &vp : vars.all()) {
		if(vp->type() != VariableType::Text) continue;
		auto *tv = static_cast<TextVariable*>(vp.get());
		QString key = vp->key();
		if(key.startsWith(prefix) && key.endsWith(suffix)) {
			// Stored key already wrapped; accept both wrapped literal and inner form
			QString inner = key.mid(prefix.size(), key.size()-prefix.size()-suffix.size());
			map[key] = tv->value();
			map[wrapKey(inner,prefix,suffix)] = tv->value();
		} else {
			map[wrapKey(key,prefix,suffix)] = tv->value();
		}
	}
	if(map.empty()) return;

	pugi::xpath_query pq("//w:p");
	auto pnodes = pq.evaluate_node_set(doc);
	for(auto &n : pnodes) {
		pugi::xml_node p = n.node();
		RunModel rm; rm.build(p);
		const QString &paraText = rm.text();
		if(paraText.isEmpty()) continue;
		QRegularExpression re(QRegularExpression::escape(prefix)+"(.*?)"+QRegularExpression::escape(suffix));
		auto it = re.globalMatch(paraText);
		struct Match { int s; int e; QString token; };
		std::vector<Match> matches; matches.reserve(4);
		while(it.hasNext()) {
			auto m = it.next();
			QString token = m.captured(0);
			if(!map.count(token)) continue; // unknown -> skip
			matches.push_back({m.capturedStart(0), m.capturedEnd(0), token});
		}
		if(matches.empty()) continue;
		// Descending order to keep offsets valid relative to original mapping
		for(auto i = (int)matches.size()-1; i>=0; --i) {
			auto &mm = matches[i];
			QString replacement = map[mm.token];
			rm.replaceRange(mm.s, mm.e, [&](pugi::xml_node w_p, pugi::xml_node styleR){
				auto newRun = RunModel::makeTextRun(w_p, styleR, replacement, true);
				return std::vector<pugi::xml_node>{ newRun };
			});
		}
	}
}

}} // namespace QtDocxTemplate::engine
