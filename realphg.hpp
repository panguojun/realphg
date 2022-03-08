/***************************************************************************
				实数PHG，也是默认的
***************************************************************************/

#define GROUP		RealPHG
#define ELEMENT		real

namespace RealPHG
{
#include "phg_head.hpp"

	inline void _PHGPRINT(const std::string& pre, real v)
	{
		PRINT(pre << v);
	}
	// ------------------------------------------
	#include "phg.hpp"
	// ------------------------------------------
	// 运算
	static var _act(code& cd, int args)
	{
		opr o = cd.oprstack.pop();

		//PRINT("act:" << o << " args = " << args)

			switch (o) {
			case '+': {
				if (args > 1) {
					var b = cd.valstack.pop();
					var a = cd.valstack.pop();
					return a + b;
				}
				else {
					return cd.valstack.pop();
				}
			}
			case '-': {
				if (args > 1) {
					var b = cd.valstack.pop();
					var a = cd.valstack.pop();
					return a - b;
				}
				else {
					return -cd.valstack.pop();
				}
			}
			case '*': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a * b;
			}
			case '/': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a / b;
			}
			case '>': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a > b;
			}
			case '<': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a < b;
			}
			case '=': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a == b;
			}
			case '&': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a && b;
			}
			case '|': {
				var b = cd.valstack.pop();
				var a = cd.valstack.pop();
				return a || b;
			}
			case '!': {
				var a = cd.valstack.pop();
				return !a;
			}
			default: {return 0; }
			}
	}

	/* tree 数据结构格式
	{
		a: 1;
		b: 2;
		{
			a: 3;
			b: 4;
		}
	}
	*/
	struct tree_t
	{
		std::map<std::string, std::string> kv;
		std::map<std::string, tree_t*> children;

		static void clear(tree_t* ot)
		{
			if (!ot) return;

			for (auto it : ot->children)
			{
				clear(it.second);
			}
			delete(ot);
		}
	};
	static inline std::string getstring(code& cd)
	{
		std::string content;
		while (!cd.eoc()) {
			char c = cd.cur();
			if (c != '\'' && c != '\"')
			{
				content += c;
				cd.next();
				continue;
			}
			cd.next();
			break;
		}
		return content;
	}
	static void _tree(code& cd, tree_t* tree, const string& pre, int depth = 0)
	{
		PRINT(pre << "{");
		cd.next();

		std::string key, val;
		std::string* pstr = &key;
		int index = 0;
		while (!cd.eoc()) {
			char c = cd.cur();
		//	PRINT("c=" << c );
			if (c == '{'|| c == '[') {
				index++;
				if (key.empty())
				{
					key = to_string(index);
				}
				tree_t* ntree = new tree_t;
				tree->children[key] = ntree;
				PRINT(pre << key << " : ");
				_tree(cd, ntree, pre + "\t", depth + 1);
				val = "";
				key = "";
				pstr = &key;
			}
			
			else if (c == ';' || c == ',' || c == '}' || c == ']') {
				if (!key.empty() || !val.empty()) {

					index++;
					if (val.empty())
					{
						val = key;
						key = to_string(index);
					}

					tree->kv[key] = val;
					PRINT(pre << key << " : " << val);

					val = "";
					key = "";
					pstr = &key;
					
				}
				cd.next();
				if (c == '}' || c == ']') {
					PRINT(pre << "}");
					return;
				}
			}
			else if (c == '\n') {
				cd.next();
			}
			else if (c == ':') {

				pstr = &val;
				cd.next();
			}
			else if (c == '\'' || c == '\"') {

				cd.next();
				*pstr += getstring(cd);
			}
			else {
				*pstr += cd.cur();
				cd.next();
			}
		}
	}
	tree_t* gtree = 0;
	static void _tree(code& cd)
	{
		//tree_t* tree = new tree_t;
		tree_t::clear(gtree);
		gtree = new tree_t;
		_tree(cd, gtree, "", 0);

		//tree_t::clear(tree);
	}

	void setup();
}

// ------------------------------------------
// API
// ------------------------------------------
static var addver(RealPHG::code& cd, int args)
{
	PRINTV(args);
	var param1 = cd.valstack.get(args - 1);
	var param2 = cd.valstack.get(args - 2);
	var param3 = cd.valstack.get(args - 3);
	PRINTV(param1);
	PRINTV(param2);
	PRINTV(param3);

	if (estack.empty())
		estack.push_back(VECLIST());

	if (verliststackpos >= estack.size())
		return 0;

	vertex v;
	v.p = vec3(param1, param2, param3);
	estack[verliststackpos].push_back(v);
	return 0;
}
static var round(RealPHG::code& cd, int args)
{
	PRINTV(args);
	var param1 = cd.valstack.get(args - 1);
	var param2 = cd.valstack.get(args - 2);
	var param3 = cd.valstack.get(args - 3);
	PRINTV(param1);
	PRINTV(param2);
	PRINTV(param3);

	if (estack.empty())
		estack.push_back(VECLIST());

	if (verliststackpos >= estack.size())
		return 0;

	roundedge(estack[verliststackpos], estack[verliststackpos][int(param1)], vec::UY, param2, param3);
	return 0;
}
static var triangi(RealPHG::code& cd, int args)
{
	PRINTV(args);
	var t1 = cd.valstack.get(args - 1);
	var t2 = cd.valstack.get(args - 2);
	var t3 = cd.valstack.get(args - 3);

	if (verliststackpos >= estack.size())
		return 0;

	if (t1 < estack[verliststackpos].size() && t2 < estack[verliststackpos].size() && t3 < estack[verliststackpos].size())
		triang0(estack[verliststackpos][t1], estack[verliststackpos][t2], estack[verliststackpos][t3]);

	return 0;
}
static var edge(RealPHG::code& cd, int args)
{
	PRINTV(args);
	for (int i = 0; i < args; i++)
	{
		int index = cd.valstack.get(args - i - 1);
		if (index >= 0 && index < gsubmesh->vertices.size())
		{
			estack[verliststackpos].push_back(gsubmesh->vertices[index]);
		}
	}
	return 0;
}
static var extrudeedge(RealPHG::code& cd, int args)
{
	var d = cd.valstack.get(args - 1);

	VECLIST& e1 = estack.back();
	vec norm = getedgenorm2(e1);
	vec dv = norm * d;
	if (weightlist.size() == e1.size())
	{
		for (int i = 0; i < e1.size(); i++)
		{
			e1[i].p += dv * blend(0, 1, weightlist[i]);
			e1[i].ind = -1;
		}
	}
	else
	{
		for (int i = 0; i < e1.size(); i++)
		{
			e1[i].p += dv;
			e1[i].ind = -1;
		}
	}
	return 0;
}

static var face(RealPHG::code& cd, int args)
{
	if (args == 2)
	{
		int s1 = cd.valstack.get(args - 1);
		int s2 = cd.valstack.get(args - 2);
		face(estack[estack.size() + s1], estack[estack.size() + s2]);
	}
	else
	{
		face(estack[estack.size() - 2], estack.back());
	}
	return 0;
}

extern void flow();
static var flow(RealPHG::code& cd, int args)
{
	flow();
	return 0;
}

extern void nform();
static var nform(RealPHG::code& cd, int args)
{
	nform();
	return 0;
}
extern void pathtest();
static var pathtest(RealPHG::code& cd, int args)
{
	pathtest();
	return INVALIDVAR;
}

var rayrender(RealPHG::code& cd, int args)
{
	renderstate = 0;
	GenSphericalCrown(vec3(0,0,25), 2, 1, 0.75);

	renderstate = 3;
	rayrender();

	return INVALIDVAR;
}
extern void test();
static var test(RealPHG::code& cd, int args)
{
	test();
	return 0;
}
// -----------------------------------
// setup
// -----------------------------------
extern void reg();
void RealPHG::setup()
{
	PRINT("setup RealPHG");

	act = _act;
	tree = _tree;

	RealPHG::register_api("ver", addver);
	RealPHG::register_api("round", round);
	RealPHG::register_api("trii", triangi);
	RealPHG::register_api("edge", edge);
	RealPHG::register_api("ext", extrudeedge);
	RealPHG::register_api("face", face);

	RealPHG::register_api("nform", nform);

	// flow
	reg();
	RealPHG::register_api("flow", flow);

	RealPHG::register_api("pathtest", pathtest);

	RealPHG::register_api("rayrender", rayrender);

	RealPHG::register_api("test", test);
}

// ====================================
// test
// ====================================
void realphg()
{
	RealPHG::dofile("main.r");
}