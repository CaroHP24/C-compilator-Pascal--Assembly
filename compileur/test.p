VAR a,b :INTEGER;
    c :DOUBLE;
    d :CHAR.

BEGIN
    a:=4;
    
    b:=5;
  
    c:=2.5;
    
    d:='d';
   
    CASE a OF 
        2: b:=2;
        6: b:=22;
    ELSE b:=28
    END;
    DISPLAY b
END.

