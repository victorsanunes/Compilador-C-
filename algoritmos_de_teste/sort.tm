(lab,24,_,_)
(asg,*1,_,i)
(lab,L1,_,_)
(menorI,i,tam,t0)
(if_f,t0,L2,_)
(asg,i,_,min)
(asg,*0,_,lixo)
(sum,i,*1,t1)
(asg,t1,_,j)
(lab,L3,_,_)
(menorI,j,tam,t2)
(if_f,t2,L4,_)
,j(asg,t3,_,auxVet1)
,min(asg,t4,_,auxVet2)
(menor,auxVet1,auxVet2,t5)
(if_f,t5,L5,_)
(asg,j,_,min)
(lab,L5,_,_)
(asg,*0,_,lixo)
(sum,j,*1,t6)
(asg,t6,_,j)
(got,L3,_,_)
(lab,L4,_,_)
(dif,i,min,t7)
(if_f,t7,L6,_)
,i(asg,t8,_,aux)
,min(asg,t9,_,aux2)
(asg,aux2,i,num)
(asg,aux,min,num)
(lab,L6,_,_)
(asg,*0,_,lixo)
(sum,i,*1,t10)
(asg,t10,_,i)
(got,L1,_,_)
(lab,L2,_,_)
(lab,94,_,_)
(asg,*9,*1,vetor)
(asg,*6,*2,vetor)
(asg,*8,*3,vetor)
(asg,*7,*4,vetor)
,vetor(param,16889856,_,_)
,*4(param,4,_,_)
(call,199,2,_)
(asg,*1,_,i)
,i(asg,t11,_,i)
,i(param,16891328,_,_)
(call,207,1,_)
