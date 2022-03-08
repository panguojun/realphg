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

	RealPHG::register_api("test", test);
}

// ====================================
// test
// ====================================
void realphg()
{
	RealPHG::dofile("main.r");
}
