'''
简易音乐下载模块。
传入参数1为下载目录，后面为下载地址
'''
from urllib import request
from multiprocessing.pool import ThreadPool
import sys
import os
# import time
host = "https://www.xzmp3.com/down/"
head = {
    "User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0",
    "Referer":host
}
pool = ThreadPool(20)
def do_download(url:str):
    '''
    为指定url进行下载
    '''
    loc=url.rfind('/')
    fp = open(url[loc+1:],"wb")
    respo = request.urlopen(request.Request(url,headers=head,method="GET"))
    # size = float(respo.headers['content-length'])
    # print("正在下载" + url[loc+1:] + ",大小为%.2fKB"%(size/1024))
    fp.write(respo.read())
    # print("已下载" + url[loc+1:])
    fp.close()
if __name__ == "__main__":
    # st = time.time()
    if len(sys.argv)<3:
        raise Exception("参数个数不足")
    loc = sys.argv[1]
    if not os.path.exists(loc):
        os.makedirs(loc)
    os.chdir(sys.argv[1])
    pool.map(do_download,set(sys.argv[2:]))
    pool.close()
    pool.join()
    # print(time.time()-st)