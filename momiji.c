//Get the Pure Data headers from: https://github.com/pure-data/pure-data/blob/master/src/m_pd.h
#include "m_pd.h"

//Create a static pointer to the class; it will be initialized in the _setup() method
static t_class *momiji_class;

//Carve out memory for the class data model and typdef it
typedef struct _momiji {
    t_object    x_obj;
    t_int       init_count, current_count;
    t_int       min_value, max_value;
    t_inlet     *in_min_value, *in_max_value, *in_reset;
    t_outlet    *out_count;
} t_momiji;

// #################### utils #####################
void momiji_reset_count(t_momiji *x){
    x->current_count = x->min_value;
}

void momiji_init_values(t_momiji *x, t_floatarg min_value, t_floatarg max_value){
    momiji_reset_count(x);
    x->min_value = min_value;
    x->max_value = max_value;
}

// #################### events handling #####################
void momiji_onBangMsg(t_momiji *x){
    if(x->current_count>x->max_value){
        x->current_count = x->current_count % (x->max_value + 1);
    }
    outlet_float(x->out_count, x->current_count);
    x->current_count = x->current_count +1;
}

void momiji_onSet_minValue(t_momiji *x, t_floatarg min_value){
    x->min_value = min_value;
}

void momiji_onSet_maxValue(t_momiji *x, t_floatarg max_value){
    x->max_value = max_value;
}

void momiji_onBang_reset(t_momiji *x){
    x->current_count = x->min_value;
}


// #################### initialization #####################
//The _new method is a class constructor
//It is required to be named [filename]_new()
void *momiji_new(t_floatarg f1, t_floatarg f2){
    //Create an instance of the polybang class
    t_momiji *x = (t_momiji *)pd_new(momiji_class);
    
    x->min_value = f1;
    x->max_value = f2;
    
    momiji_reset_count(x);
    
    // inlets init
    x->in_min_value = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("RATIO_A"));
    x->in_max_value = inlet_new(&x->x_obj, &x->x_obj.ob_pd, &s_float, gensym("RATIO_B"));
    x->in_reset = inlet_new(&x->x_obj, &x->x_obj.ob_pd, gensym("bang"), gensym("bang2"));

    // outlets init
    x->out_count = outlet_new(&x->x_obj, &s_float);
    
    //Return the instance
    return (void *)x;
}

void momiji_free(t_momiji *x){
    inlet_free(x->in_min_value);
    inlet_free(x->in_max_value);
    inlet_free(x->in_reset);
    outlet_free(x->out_count);
}

//The _setup method initializes the class in memory
//This method is the place where class behaviors are added
//It is required to be named [filename]_setup()
void momiji_setup(void){
    //Initialize the class
    momiji_class = class_new(gensym("momiji"),
                               (t_newmethod)momiji_new,
                               (t_method)momiji_free,
                               sizeof(t_momiji),
                               CLASS_DEFAULT,
                               A_DEFFLOAT, //MIN of A:B
                               A_DEFFLOAT, //MAX of A:B
                               0);
    //Bang
    class_addbang(momiji_class, (t_method)momiji_onBangMsg);
    
    class_addmethod(momiji_class,
                    (t_method)momiji_onSet_minValue,
                    gensym("RATIO_A"),
                    A_DEFFLOAT, //A of A:B
                    0);
    
    class_addmethod(momiji_class,
                    (t_method)momiji_onSet_maxValue,
                    gensym("RATIO_B"),
                    A_DEFFLOAT, //A of A:B
                    0);
    class_addmethod(momiji_class, (t_method)momiji_onBang_reset, gensym("bang2"), 0);
}

