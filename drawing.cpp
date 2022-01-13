// This inclusion is necessary, or LEDA's includes will get mad.
#include <limits.h>
// -------------------------------------------------------------
#include <LEDA/core/list.h>
#include <LEDA/geo/point.h>
#include <LEDA/geo/circle.h>
#include <LEDA/geo/random_point.h>
#include <LEDA/geo/geo_alg.h>
#include <LEDA/graphics/window.h>
#include <LEDA/graphics/color.h>
#include <queue>
#include <math.h>
using namespace leda;

class ch_edge;
static ch_edge* last_edge = NULL;
static window W;


class ch_edge{
public:
	point source, target;
	ch_edge* succ;
	ch_edge* pred;
	bool outside;
	ch_edge(const point& a, const point& b) : source(a), target(b){
		outside = true;
		last_edge = this;
	}
};

list<point> Convexhull(const list<point> L)
{	
	// less than three points
	if (L.length() < 2) return L;
	
	// more than two points
	list<point> CH;
	point a = L.head();
	point b = L.head();
	point c, p;
	
	// test if all points are equal
	if(a==b){
		forall(p,L) if (p!=a) {b=p; break;}
	}
	if(a==b){
		//all points are equal
		CH.append(a);
		return CH;
	}
	
	// find c
	int orient;
	forall(c,L) if((orient = orientation(a,b,c)) != 0) break;
	if(orient == 0){
		// all points are collinear
		forall(p,L){
			if (compare(p,a) < 0) a = p;
			if (compare(p,b) > 0) b = p;
		}
		CH.append(a);
		CH.append(b);
		return CH;
	}
	
	//a, b, c are not collinear
	if(orient < 0) leda_swap(b,c);
	last_edge = NULL;
	ch_edge* T[3];
	T[0] = new ch_edge(a,b);
	T[1] = new ch_edge(b,c);
	T[2] = new ch_edge(c,a);

	int i;
	for(i = 0; i<3;i++){
		W.draw_segment(T[i]->source,T[i]->target);
	}
	for(i = 0; i< 2; i++) T[i] -> succ = T[i+1];
	T[2] -> succ = T[0];

	for(i = 0; i < 3; i++) T[i] -> pred = T[i-1];
	T[0] -> pred = T[2];
	
	forall(p,L){
		int i = 0;
		while(i < 3 && !right_turn(T[i]->source, T[i]->target, p)) i++;
		if(i == 3) 
		{ continue;}
	
		ch_edge* e = T[i];
		while(!e->outside){
			ch_edge* r0 = e->pred;
			if (right_turn(r0->source, r0->target, p)) e = r0;
			else{
				ch_edge* r1 = e->succ;
				if (right_turn(r1->source, r1->target,p)) e =r1;
				else {e = NULL; break;}
			}
		}
		if (e == NULL) continue; //p inside current hull
		
		W.set_node_width(4);
		W.draw_filled_node(p,green);
		W.set_node_width(2);
		//compute upper tangent
		ch_edge* high = e->succ;
		while(orientation(high->source, high->target, p)<=0) high=high->succ;

		//compute lower tangent
		ch_edge* low = e->pred;
		while(orientation(low->source, low->target,p)<= 0) low = low->pred;
		e = low->succ;

		//add new tangents between low and high
		ch_edge* e_l = new ch_edge(low->target,p);
		ch_edge* e_h = new ch_edge(p, high->source);
		e_h -> succ = high;
		e_l -> pred = low;
		high->pred = e_l->succ = e_h;
		low->succ = e_h ->pred = e_l;
		
		W.read_mouse();			
		W.draw_segment(low->target,p);
		W.draw_segment(p, high->source);
		W.read_mouse();
		//mark edges between low and high as inside
		//and defien refinements
		while(e != high){
			ch_edge* q = e->succ;
			e->pred = e_l;
			e->succ = e_h;
			e->outside = false;
			W.draw_segment(e->source, e->target, white);
			W.draw_filled_node(e->source);
			W.draw_filled_node(e->target);
			e = q;
		}
	
		W.set_node_width(4);
		W.draw_filled_node(p,white);
		W.set_node_width(2);
		W.draw_filled_node(p);
	
	}
	
	ch_edge* l_edge = last_edge;

	CH.append(l_edge -> source);
	for(ch_edge* e = l_edge->succ; e!=l_edge; e=e->succ)
		CH.append(e->source);
	return CH;
}


int main()
{
	//window W;
	W.init(-210,210,-210);
	W.display();
	W.set_node_width(2);
	
	W.draw_point(0,0,white);
	W.read_mouse();
	
	list<point> L;
	random_points_in_square(16, 200, L);
	L.sort();
	point p;
	forall(p,L) W.draw_filled_node(p);
	W.read_mouse();
	
	list<point> CH;
	CH = Convexhull(L);
	W.set_node_width(3);
	W.set_line_width(2.5);
	forall(p,CH) W.draw_filled_node(p,red);
	W.draw_pixels(L);	
	
	segment s;
	s = segment(CH.back(),CH.front());
	list<segment> S;
	while(!CH.empty()){
		point m = CH.pop();
		if(!CH.empty()){
			point n = CH.front();
			S.append(segment(m,n));	
		}
	}
	
	W.read_mouse();	
	W.draw_segment(s,red);
	forall(s,S) W.draw_segment(s,red);
			
	W.read_mouse();
	W.close();
}
