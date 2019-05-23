class Base {
    attr:Int <- 5;
    meth():Int{
        attr
    };
};

class Inh inherits Base {
    attr1:Int <- 3;
    meth():Int{
        attr1 + attr
    };
};

class Main inherits IO {
    inh: Inh <- new Inh;
    base: Base <- new Base;
    main():Int{{
        out_int(inh.meth());
        out_int(base.meth());
        0;
   }};
};
