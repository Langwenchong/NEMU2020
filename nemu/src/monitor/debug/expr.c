#include "nemu.h"

/* We use the POSIX regex functions to process regular expressions.
 * Type 'man regex' for more information about POSIX regex functions.
 */
#include <sys/types.h>
#include <regex.h>
uint32_t eval(int p,int q);
enum {
	NOTYPE = 256, EQ,NEQ,AND,OR,NUMBER,HNUMBER,POINTER,REGISTER,MINUS,MARK

	/* TODO: Add more token types */

};

static struct rule {
	char *regex;
	int token_type;
	int priority;
} rules[] = {

	/* TODO: Add more rules.
	 * Pay attention to the precedence level of different rules.
	 */

	{" +",	NOTYPE,0},				// spaces
	{"	",NOTYPE,0},				//tabs
	{"\\+", '+',4},					// plus
	{"==", EQ,3},					// equal
	{"!=",NEQ,3},					//not qual
	{"&&",AND,2},					//and
	{"\\|\\|",OR,1},					//or
	{"!",'!',6},					//not
	{"\\b[0-9]+\\b",NUMBER,0},			//number
	{"\\b0[xX][0-9a-fA-F]+\\b",HNUMBER,0},		//hex number
	{"-",'-',4},					//sub
	{"\\*",'*',5},					//mult
	{"/",'/',5},					//div
	{"\\(",'(',7},					//left bracket
	{"\\)",')',7},					//right bracket
	{"\\$[a-zA-Z]+",REGISTER,0},			//register
	{"\\b[a-zA-Z_0-9]+",MARK,0},			//mark
};
#define NR_REGEX (sizeof(rules) / sizeof(rules[0]) )

static regex_t re[NR_REGEX];

/* Rules are used for many times.
 * Therefore we compile them only once before any usage.
 */
void init_regex() {
	int i;
	char error_msg[128];
	int ret;

	for(i = 0; i < NR_REGEX; i ++) {
		ret = regcomp(&re[i], rules[i].regex, REG_EXTENDED);
		if(ret != 0) {
			regerror(ret, &re[i], error_msg, 128);
			Assert(ret == 0, "regex compilation failed: %s\n%s", error_msg, rules[i].regex);
		}
	}
}

typedef struct token {
	int type;
	char str[32];
	int priority;
} Token;

Token token[32];
int nr_token;

static bool make_token(char *e) {
	int position = 0;
	int i;
	regmatch_t pmatch;
	
	nr_token = 0;

	while(e[position] != '\0') {
		/* Try all rules one by one. */
		for(i = 0; i < NR_REGEX; i ++) {
			if(regexec(&re[i], e + position, 1, &pmatch, 0) == 0 && pmatch.rm_so == 0) {
				char *substr_start = e + position;
				char *dest=e+position+1;
				int substr_len = pmatch.rm_eo;

				Log("match rules[%d] = \"%s\" at position %d with len %d: %.*s", i, rules[i].regex, position, substr_len, substr_len, substr_start);

				/* TODO: Now a new token is recognized with rules[i]. Add codes
				 * to record the token in the array `tokens'. For certain types
				 * of tokens, some extra actions should be performed.
				 */

				switch(rules[i].token_type) {
					case NOTYPE:break;
					case REGISTER:
						    token[nr_token].type=rules[i].token_type;
						    token[nr_token].priority=rules[i].priority;
						    strncpy(token[nr_token].str,dest,substr_len-1);
						    token[nr_token].str[substr_len-1]='\0';
						    nr_token++;
						    break;
					default: 
						    token[nr_token].type=rules[i].token_type;
						    token[nr_token].priority=rules[i].priority;
						    strncpy(token[nr_token].str,substr_start,substr_len);
						    token[nr_token].str[substr_len]='\0';
						    nr_token++;
						    break;
				}
				position+=substr_len;
				break;
			}
		}

		if(i == NR_REGEX) {
			printf("no match at position %d\n%s\n%*.s^\n", position, e, position, "");
			return false;
		}
	}

	return true; 
}

uint32_t expr(char *e, bool *success) {
	if(!make_token(e)) {
		*success = false;
		return 0;
	}
	int i=0;
	for(i=0;i<nr_token;i++){
		if(token[i].type=='*'&&(i==0||(token[i-1].type!=NUMBER&&token[i-1].type!=HNUMBER&&token[i-1].type!=REGISTER&&token[i-1].type!=MARK&&token[i-1].type!=')'))){
			token[i].type=POINTER;
			token[i].type=6;
		}
		if(token[i].type=='-'&&(i==0||(token[i-1].type!=NUMBER&&token[i-1].type!=HNUMBER&&token[i-1].type!=REGISTER&&token[i-1].type!=MARK&&token[i-1].type!=')'))){
			token[i].type=MINUS;
			token[i].priority=6;
		}
	}
	/* TODO: Insert codes to evaluate the expression. */
	*success=true;
	return eval(0,nr_token-1);
}

bool check_parentheses(int p,int q){
	int i=0;
	if(token[p].type=='('&&token[q].type==')'){
		int lc=0,rc=0;
		for(i=p+1;i<q;i++){
			if(token[i].type=='(')lc++;
			if(token[i].type==')')rc++;
			if(rc>lc)return false;
		}
		if(lc==rc)return true;
	}
	return false;
}

int dominant_operator(int p,int q){
	int i,j;
	int min_priority=10;
	int loc=p;
	for(i=p;i<=q;i++){
		if(token[i].type==NUMBER||token[i].type==HNUMBER||token[i].type==REGISTER||token[i].type==MARK){
			continue;
		}
		int cnt=0;
		bool key=true;
		for(j=i-1;j>=p;j--){
			if(token[j].type=='('&&!cnt){
				key=false;
				break;
			}
			if(token[j].type=='(')cnt--;
			if(token[j].type==')')cnt++;
		}
		if(!key)continue;
		if(token[i].priority<=min_priority){
			min_priority=token[i].priority;
			loc=i;
		}
	}
	return loc;
}

uint32_t  eval(int p,int q){
	if(p>q){
		Assert(p>q,"Bad expression!");
		return 0;
	}
	else if(p==q){
		int num=0;
		if(token[p].type==NUMBER)sscanf(token[p].str,"%d",&num);
		if(token[p].type==HNUMBER)sscanf(token[p].str,"%x",&num);
		if(token[p].type==REGISTER){
			if(strlen(token[p].str)==3){
				int i;
				for(i=R_EAX;i<=R_EDI;i++){
					if(strcmp(token[p].str,regsl[i])==0)break;
					if(i>R_EDI){
						if(strcmp(token[p].str,"eip")==0)num=cpu.eip;	
						else{
							Assert(1,"No this register!");
						}
					}
					else{
						num=reg_l(i);
					}
				}
			}
			else if(strlen(token[p].str)==2){
				if(token[p].str[1]=='x'||token[p].str[1]=='p'||token[p].str[1]=='i'){
					int i;
					for(i=R_AX;i<=R_DI;i++){
						if(strcmp(token[p].str,regsw[i])==0)break;
						num=reg_w(i);
					}
				}
				else if(token[p].str[1]=='l'||token[p].str[1]=='h'){
					int i;
					for(i=R_AL;i<=R_BH;i++){
						if(strcmp(token[p].str,regsb[i])==0)break;
						num=reg_b(i);
					}
				}
				else assert(1);
			}
		}
		return num;
	}
	else if(check_parentheses(p,q)==true){
		return eval(p+1,q-1);
	}
	else{
		int op=dominant_operator(p,q);
		if(p==op||token[op].type==POINTER||token[op].type==MINUS||token[op].type=='!'){
			int val=eval(p+1,q);
			switch(token[p].type){
				case POINTER:return swaddr_read(val,4);
				case MINUS:return -val;
				case '!':return !val;
				default:Assert(1,"default\n");
			}
		}
		int val1=eval(p,op-1);
		int val2=eval(op+1,q);
		switch(token[op].type){
			case '+':return val1+val2;
			case '-':return val1-val2;
			case '*':return val1*val2;
			case '/':return val1/val2;
			case EQ:return val1==val2;
			case NEQ:return val1!=val2;
			case AND:return val1&&val2;
			case OR:return val1||val2;
			default:
				break;
		}
	}
	assert(1);
	return -1234567;
}
