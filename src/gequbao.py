'''通过 www.gequbao.com 查询'''
from urllib import request,parse
from lxml import etree
from multiprocessing.pool import ThreadPool
host = "https://www.gequbao.com"
head = {
    "User-Agent" : "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:91.0) Gecko/20100101 Firefox/90.0",
    "Accept-Language" : "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2",
    "Referer":host
}
pool = ThreadPool(20)
_fp = open("links.tmp","w",encoding="utf-8")
def _redirect_addr(addr:str):
    response = request.urlopen(request.Request(url=addr,headers=head,method="GET"))
    return response.geturl()
def _text(respo):
    return respo.read().decode("utf-8")
def get_list(name:str):
    '''
   Parameters
    ----------
    name : str
        歌曲名，要求以字符串形式给出

    Returns
    -------
    None.
    '''
    count = 0
    pages = []
    uri = host + "/s/" + parse.quote(name)
    respo = request.urlopen(request.Request(uri,headers=head,method="GET"))
    tree = etree.HTML(_text(respo))
    result = tree.xpath("//body/div[1]/div[2]/div/div/table//td[1]/a[1]")
    for item in result:
        pages.append(host + item.xpath("./@href")[0])
        count += 1
        if count >= 20:
            break
    # print(len(pages))
    pool.map(get_addr,pages)
    pool.close()
    pool.join()
    _fp.close()
def get_addr(page_url:str):
    respo = request.urlopen(request.Request(page_url,headers=head,method="GET"))
    tree = etree.HTML(_text(respo))
    fake_url = tree.xpath("//body/div[1]/script/text()")
    fake_url = fake_url[0]
    start = fake_url.find("http")
    end = fake_url.find("\'.")
    title = tree.xpath("//body/div[1]/div[2]/div/div[1]/span[1]/text()")
    singer = tree.xpath("//body/div[1]/div[2]/div/div[1]/span[2]/small/text()")
    if not _redirect_addr(fake_url[start:end]).endswith("/404"):
        _fp.write(title[0] + ';' + singer[0] + ';' +  fake_url[start:end] + '\n')
    # print(fake_url[start:end])