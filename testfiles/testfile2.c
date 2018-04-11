#define TEST_OBJ_MACRO "test message"
#define TEST_FUNC_MACRO(name, a) void f##name(){return a;}
#ifdef TEST_MACRO
#endif

TEST_FUNC_MACRO(test,1)
static inline int f(int \u7834){return \u7834;}
// commments \
comments
int main(){
    _Static_assert(1+1==2,TEST_OBJ_MACRO);
    do{}while(0);
    int arg=0xFFull;
    return f(arg+++arg);
}
