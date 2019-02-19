#coding:utf-8
s = raw_input()
f = open('m/TCP4/localhost/8888/opt','w+')
while (1) :
    print('送信する文字を入力してください')
    s = raw_input()
    f.write(s)
    data1 = f.read(256) 
    print(data1) # 文字列データ
f.close()
