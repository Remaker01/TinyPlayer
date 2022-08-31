# -*- coding: utf-8 -*-
"""
Created on Sun Jul  3 21:15:00 2022

@author: Admin
"""
# 由于站点可能存在变化，这个文件可能要经常修改
from urllib import request,parse
import sys
from lxml import etree
# import time
from multiprocessing.dummy import Pool
host = "https://www.xzmp3.com"
head = {
    "User-Agent" : "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:91.0) Gecko/20100101 Firefox/90.0",
    "Accept-Language" : "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2",
    "Referer":host
}
pool = Pool(20)
fp = open("links.tmp","w",encoding="utf-8")
# 第一步：获取下载页面的URLs
def get_downpage_list(name:str):
    search_page = host + "/xiazai/"
    uri = search_page + parse.quote(name) + ".htm"
    response = request.urlopen(request.Request(uri,headers=head,method="GET"))
    page_text = response.read().decode("utf-8")
    tree = etree.HTML(page_text)
    # html/body/div/div[2]/div/div[2](截止到这里是"body")/div[1]/div/div[3]/div[1]/div[i]/a
    result = tree.xpath("//body/div/div[2]/div/div[2]//div[@class=\"list_row\"]")
    #print(result)
    #开始查找urls
    url_list = []
    for each in result:
        location = each.xpath("./div[1]/a/@href")
        title = each.xpath("./div[1]/a/text()")
        singer = each.xpath("./div[2]/a/text()")
        if len(location)>0:
            url_list.append(title[0] + ';' + singer[0] + ';' + host+location[0])
    pool.map(get_downlink,url_list)
# 第二步：从获取到的URLs中提取实际下载地址,保存在文件中
def get_downlink(info:str):
    lst = info.split(';')
    response = request.urlopen(request.Request(url=lst[2],headers=head,method="GET"))
    page_text = response.read().decode("utf-8")
    tree = etree.HTML(page_text)
    result_addr = tree.xpath("//body/div/div[2]/div[1]/div[2]/div[1]/div/div[2]/div[1]/a")
    # fp.write(result_addr[0].xpath("./@href")[0])
    response = request.urlopen(request.Request(url=result_addr[0].xpath("./@href")[0],headers=head,method="GET"))
    true_url = response.geturl()
    if(true_url.find(host) < 0 and (not true_url.endswith("/404"))):
        fp.write(lst[0] + ';' + lst[1] + ';' +  true_url + '\n')
if __name__ == "__main__":
    if len(sys.argv) < 2:
        fp.close()
        raise Exception("错误：未指定关键词")
    # st = time.time()
    get_downpage_list(sys.argv[1])
    # print(time.time() - st)
    fp.close()