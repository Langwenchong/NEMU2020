#include "monitor/watchpoint.h"
#include "monitor/expr.h"
#include "nemu.h"

#define NR_WP 32

static WP wp_pool[NR_WP];
static WP *head, *free_;

void init_wp_pool() {
	int i;
	for(i = 0; i < NR_WP; i ++) {
		wp_pool[i].NO = i;
		wp_pool[i].next = &wp_pool[i + 1];
	}
	wp_pool[NR_WP - 1].next = NULL;

	head = NULL;
	free_ = wp_pool;
}

/* TODO: Implement the functionality of watchpoint */

WP *new_wp(){
	WP *q,*p;
	q=free_;
	free_=free_->next;
	q->next=NULL;
	p=head;
	if(p==NULL){
		head=q;
		p=head;
	}
	else{
		while(p->next!=NULL){
			p=p->next;
		}
		p->next=q;
	}
	return q;
}

void free_wp(WP *wp){
	WP *q, *p;
	p=free_;
	if(p==NULL){
		free_=wp;
		p=free_;
	}
	else{
		while(p->next!=NULL){
			p=p->next;
		}
		p->next=wp;
	}
	q=head;
	if(head==NULL)assert(0);
	if(head->NO==wp->NO){
		head=head->next;
	}
	else{
		while(q->next->NO!=wp->NO && q->next!=NULL){
			q=q->next;
		}
		if(q->next==NULL&&q->NO==wp->NO){
			printf("Wrong!");
		}
		else if(q->NO==wp->NO){
			q->next=q->next->next;
		}
		else assert(0);
	}
	wp->next=NULL;
	wp->value=0;
	wp->b=0;
	wp->expr[0]='\0';
}

void delete_wp(int n){
	WP *q;
	q=&wp_pool[n];
	free_wp(q);
}

void print_wp(){
	WP *q;
	q=head;
	while(q!=NULL){
		printf("Watchpoint at  %d: %s = %d\n",q->NO,q->expr,q->value);
		q=q->next;
	}
}

bool check_wp(){
	WP *q;
	q=head;
	bool ans=true;
	bool success;
	while(q!=NULL){
		uint32_t EXPR=expr(q->expr,&success);
		if(!success)assert(1);
		if(EXPR!=q->value){
			ans=false;
			if(q->b){
				printf("Hit breakpoint %d at 0x%08x\n", q->b,cpu.eip);
				q=q->next;
				continue;
			}
			printf("Watchpoint %d: %s\n",q->NO,q->expr);
			printf("Old value: %d\n", q->value);
			printf("New value: %d\n",EXPR);
			q->value=EXPR;
		}
		q=q->next;
	}
	return ans;
}

