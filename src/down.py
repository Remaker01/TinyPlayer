'''
简易音乐下载模块。
传入参数1为下载目录，后面为下载地址与文件名，用英文分号分隔
'''
from urllib3.poolmanager import PoolManager
from multiprocessing.pool import ThreadPool
import sys
import os
# import time
head = {
    "User-Agent":"Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:90.0) Gecko/20100101 Firefox/90.0",
    "Accept":"gzip, deflate",
    "Connection":"keep-alive"
}
_http = PoolManager(num_pools=5)
pool = ThreadPool(10)
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
    respo = _http.request("GET",url=url,headers=head,preload_content=False)
    for chunk in respo.stream(1024):
        fp.write(chunk)
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
    for i in range(2,argc):
        pool.apply_async(do_download,args=(sys.argv[i],))
    pool.close()
    pool.join()
    _http.clear()
    # print(time.time()-st)