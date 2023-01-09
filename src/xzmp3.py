# -*- coding: utf-8 -*-
'''通过 www.xzmp3.com 查询'''
# 由于站点可能存在变化，这个文件可能要经常修改
from urllib3.poolmanager import PoolManager
from lxml import etree
from multiprocessing.pool import ThreadPool
host = "https://www.xzmp3.com"
head = {
    "User-Agent" : "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:91.0) Gecko/20100101 Firefox/91.0",
    "Accept-Language" : "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2",
    "Referer":host,
    "Connection":"keep-alive"
}
pool = ThreadPool(20)
_http = PoolManager(num_pools=5,headers=head)
__fp = open("links.tmp","w",encoding="utf-8")
def _text(respo):
    return respo.data.decode("utf-8")
# 第一步：获取下载页面的URLs
def get_downpage_list(name:str):
    search_page = host + "/xiazai/"
    uri = search_page + name + ".htm"
    response = _http.request(url=uri,method="GET")
    page_text = _text(response)
    tree = etree.HTML(page_text)
    # html/body/div/div[2]/div/div[2](截止到这里是"body")/div[1]/div/div[3]/div[1]/div[i]/a
    result = tree.xpath("//body/div/div[2]/div/div[2]//div[@class=\"list_row\"]")
    #print(result)
    #开始查找urls
    url_list = []
    count = 0
    for each in result:
        location = each.xpath("./div[1]/a/@href")
        title = each.xpath("./div[1]/a/text()")
        singer = each.xpath("./div[2]/a/text()")
        if len(location)>0:
            url_list.append(title[0] + ';' + singer[0] + ';' + host+location[0])
            count+=1
        if count>=20:
            break
    pool.map(get_downlink,url_list)
    pool.close()
    pool.join()
    __fp.close()
# 第二步：从获取到的URLs中提取实际下载地址,保存在文件中
def get_downlink(info:str):
    lst = info.split(';')
    response = _http.urlopen(url=lst[2],method="GET") # 提取url，发起请求
    page_text = _text(response)
    tree = etree.HTML(page_text)
    result_addr = tree.xpath("//body/div/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[1]/a")[0]
    response = _http.urlopen(url=result_addr.xpath("./@href")[0],method="GET",preload_content=False) # 提取真实音乐地址，发起请求，校验是否有效
    lst[2] = response.geturl()
    if(lst[2].find(host) < 0 and (not lst[2].endswith("/404"))):
        __fp.write((";".join(lst)) + '\n')
    response.release_conn()