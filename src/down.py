'''
简易音乐下载模块。
传入参数1为下载目录，后面为下载地址与文件名，用英文分号分隔
'''
from urllib import request
from multiprocessing.pool import ThreadPool
import sys
import os
# import time
head = {
    "User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0",
    "Accept":"gzip, deflate"
}
pool = ThreadPool(20)
argc = len(sys.argv)
ILLEGAL_CHARS=[':','*','?','\"','<','>','|']
def _get_url_fname(urls:str):
    tmp=urls.split(';')
    if len(tmp)==1:
        url,fname = tmp[0],urls[urls.rfind('/')+1:]
    else:
        url,fname=tmp[0],tmp[1]
	# 去掉文件名中的非法字符
        fname = ''.join(x for x in fname if x.isprintable() and (x not in ILLEGAL_CHARS))
    return url,fname
def do_download(urls:str):
    '''
    为指定url进行下载
    '''
    url,fname = _get_url_fname(urls)
    fp = open(fname,"wb")
    respo = request.urlopen(request.Request(url,headers=head,method="GET"))
    # size = float(respo.headers['content-length'])
    # print("正在下载" + url[loc+1:] + ",大小为%.2fKB"%(size/1024))
    fp.write(respo.read())
    # print("已下载" + url[loc+1:])
    fp.close()
if __name__ == "__main__":
    # st = time.time()
    if argc<3:
        raise Exception("参数个数不足")
    loc = sys.argv[1]
    if not os.path.exists(loc):
        os.makedirs(loc)
    os.chdir(loc)
    for i in range(2,argc,10):
        urls=set(sys.argv[i:min(argc,i+10)])
        pool.map(do_download,urls)
    pool.close()
    pool.join()
    # print(time.time()-st)