VAR a,b :INTEGER;
    c :DOUBLE;
    d :CHAR.

    a:=2*2;
    DISPLAY a;
    b:=5;
    DISPLAY b;
    c:=2.5;
    DISPLAY c;
    d:='d';
    DISPLAY d;
    DISPLAY '=';
    DISPLAY(a>3);
    FOR a:= 4 DOWNTO 1 DO
        BEGIN
            b:=b+1;
            DISPLAY b
        END;
    DISPLAY b.
