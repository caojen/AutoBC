import json
import re

# 每个状态,去除多余的空格和末尾的&
def getState(state):
    while(state[0] == ' '):
        state=state[1:]
    while(state[-1] == ' ' or state[-1] == '&'):
        state=state[:-1]
    return state

def parseLassoBC(str):
    # print(str)
    prefix = []
    suffix = []
    dic = {}
    current_prefix = ''
    parsingPrefix = True
    for i in range(0,len(str)):
        if(str[i] == '(' or str[i] == 'F'or str[i] == ')'):
            continue
        elif(str[i] == 'X'):
            prefix.append(getState(current_prefix))
            current_prefix = ''
        elif(str[i] == 'G'):
            parsingPrefix = False
        else:
            current_prefix += str[i]
    if(parsingPrefix):
        prefix.append(getState(current_prefix))
    else:
        suffix.append(getState(current_prefix))
    dic['prefix'] = prefix
    dic['suffix'] = suffix
    for l in dic['prefix']:
      print(l)
    return dic

# 反向解析出前后缀公式, k个lassoBC的析取截k个出来，然后每个lassoBC取状态
def parse_kLassoBc(lassoBc):
    LassoBC = []
    matchObj = re.match( r'\((.*)\)\|\((.*)\)', lassoBc)
    latter_part = ''
    left = ''
    while(matchObj):
        left = matchObj.group(1)
        right = matchObj.group(2)
        if(right.find("F")!=-1):
            LassoBC.append(right)
        else:
            latter_part = right + latter_part
            if(latter_part.find('F')!=-1):
                LassoBC.append(latter_part)
                latter_part = ''
        matchObj = re.match( r'\((.*)\)\|\((.*)\)', left)
    LassoBC.append(left)
    result = []
    for lassoBc in LassoBC:
        result.append(parseLassoBC(lassoBc))
    # print(result)

import sys

def test():
    # input_LassoBc = "F((h)&(m))"
    input_LassoBc = sys.argv[1]
    parse_kLassoBc(input_LassoBc)

test()
