LTL :=  Proposition |
        LTLOp1 Proposition |
        Proposition1  LTLOp2  Proposition2 |

        LTLOp1 LTL |
        LTL1 LTLOp2 LTL2

Proposition := Literal |
        ProOp1 Literal |
        Literal1 ProOp2 Literal2

Literal := a | b | c | ... | str1 | str2 | ...

LTLOp1 := 'Next' | 'Finally' | 'Global'

LTLOp2 := 'Until' | 'Release'

ProOp1 := 'Not'

ProOp2 := 'And' | 'Or'
