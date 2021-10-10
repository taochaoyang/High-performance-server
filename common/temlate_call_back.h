#ifndef _TEMLATE_CALL_BACK_H
#define _TEMLATE_CALL_BACK_H
template<typename PR_OBJ, typename FUNC, typename ARG>
struct template_call_back_with_arg{
    PR_OBJ pr_obj;
    FUNC func;
    ARG arg;

    static void *call_back(void *call_back_arg) {
        using type_call_back = template_call_back_with_arg<PR_OBJ, FUNC, ARG>;
        type_call_back instance = *(type_call_back *)call_back_arg;
        PR_OBJ pr_obj = instance.pr_obj;
        FUNC func = instance.func;
        ARG arg = instance.arg;    
        (pr_obj->*func)(arg);
    }
};


template<typename PR_OBJ, typename FUNC, typename ARG1, typename ARG2>
struct template_call_back_with_two_arg{
    PR_OBJ pr_obj;
    FUNC func;
    ARG1 arg1;
    ARG2 arg2;

    static void *call_back(void *call_back_arg) {
        using type_call_back = template_call_back_with_two_arg<PR_OBJ, FUNC, ARG1, ARG2>;
        type_call_back instance = *(type_call_back *)call_back_arg;
        PR_OBJ pr_obj = instance.pr_obj;
        FUNC func = instance.func;
        ARG1 arg1 = instance.arg1;
        ARG2 arg2 = instance.arg2;    
        (pr_obj->*func)(arg1, arg2);
    }
};

template<typename PR_OBJ, typename FUNC>
struct template_call_back_without_arg{
    PR_OBJ pr_obj;
    FUNC func;

    static void *call_back(void *call_back_arg) {
        using type_call_back = template_call_back_without_arg<PR_OBJ, FUNC>;
        type_call_back instance = *(type_call_back *)call_back_arg;
        PR_OBJ pr_obj = instance.pr_obj;
        FUNC func = instance.func;
        (pr_obj->*func)();        

    }
};



#endif