/**
 * Author: Emil Lenngren
 * Date: 2011-11-29
 * Source:
 * Description: Calculates a valid assignment to boolean variables a, b, c,... to a 2-SAT problem, so that an expression of the type $(a\|\|b)\&\&(!a\|\|c)\&\&(d\|\|!b)\&\&...$ becomes true, or reports that it is unsatisfiable.
 * Usage:
 *  TwoSat ts(number of boolean variables);
 *  ts.add_clause(0, true, 3, false); // Var 0 is true or var 3 is false (or both)
 *  ts.set_value(2, true); // Var 2 is true
 *  ts.at_most_one({0,1,2}); // <= 1 of vars 0, 1 and 2 are true
 *  ts.solve(); // Returns true iff it is solvable
 *  ts.values[0..N-1] holds the assigned values to the vars
 * Time: O(N+E), where N is the number of boolean variables, and E is the number of clauses.
 */
#pragma once

#include <vector>

using namespace std;

struct TwoSat {
	vector<char> values; //0 = false, 1 = true
	int N;

	struct Node {
		vi ins, outs;
		int comp_id, vis;
		Node() : vis(0) {}
	};
	vector<Node> nodes;

	TwoSat(int n = 0) : values(n), N(n), nodes(2*n) {}

	int add_var() { // (optional)
		values.emplace_back();
		nodes.emplace_back();
		nodes.emplace_back();
		return N++;
	}

	void add_clause(int a_index, bool a_value, int b_index, bool b_value) {
		int a = 2*a_index + a_value, b = 2*b_index + b_value;
		nodes[a^1].outs.push_back(b), nodes[b].ins.push_back(a^1);
		nodes[b^1].outs.push_back(a), nodes[a].ins.push_back(b^1);
	}

	void set_value(int index, bool value) { // (optional)
		add_clause(index, value, index, value);
	}

	void at_most_one(const vi& li, bool val=1) { // (optional)
		if (sz(li) <= 1) return;
		int cur = li[0];
		rep(i,2,sz(li)) {
			int next = add_var();
			add_clause(cur, !val, li[i], !val);
			add_clause(cur, !val, next, val);
			add_clause(li[i], !val, next, val);
			cur = next;
		}
		add_clause(cur, !val, li[1], !val);
	}

	struct Comp {
		vi nodes;
		bool value;
		Comp() : value(0) {}
	};
	vector<Comp> comp;
	vi st;

	void dfs1(int v){
		Node& n = nodes[v];
		if (n.vis) return;
		n.vis = 1;
		for (int e : n.outs) dfs1(e);
		st.push_back(v);
	}

	void dfs2(int v){
		nodes[v].vis = 1;
		nodes[v].comp_id = sz(comp)-1;
		comp.back().nodes.push_back(v);
		for (int e : nodes[v].ins) if (!nodes[e].vis) dfs2(e);
	}

	bool solve(){
		st.reserve(2*N);
		rep(i, 0, 2*N) dfs1(i);
		rep(i, 0, 2*N) nodes[i].vis = 0;
		while(!st.empty()){
			if (!nodes[st.back()].vis){
				comp.push_back(Comp());
				dfs2(st.back());
			}
			st.pop_back();
		}
		for (Comp& d : comp) {
			if (d.value) continue;
			Comp& c = comp[nodes[d.nodes.front()^1].comp_id];
			if (&c == &d) return 0;
			c.value = 1;
			for (int e : d.nodes) values[e/2] = !(e%2);
		}
		return 1;
	}
};
