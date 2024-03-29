'''音乐搜索工具'''
from urllib3.poolmanager import PoolManager
#TODO: 可否在执行_write时做一些并发
import ujson as json
from urllib.parse import quote,urlsplit
HOST = "http://www.zhiyunge.cn/music/"
# ACCEPT_WEB = "text/html,application/xhtml+xml,application/xml;q=0.9,image/avif,image/webp,*/*;q=0.8"
ACCEPT_JSON = "application/json, text/javascript, */*; q=0.01"
PROVIDORS = ("netease","kugou","qq","lizhi","baidu")
_head = {
    "Accept-encoding":"gzip, deflate, br",
    "Accept-Language" : "zh-CN,zh;q=0.8,zh-TW;q=0.7,zh-HK;q=0.5,en-US;q=0.3,en;q=0.2",
    "Connection":"keep-alive",
    "User-Agent" : "Mozilla/5.0 (Windows NT 10.0; Win64; x64; rv:92.0) Gecko/20100101 Firefox/92.0",
}
_http = PoolManager(num_pools=5)
def _get_links_from_json(respo):
    objs = json.loads(respo).get("data")
    if objs is None:
        return []
    songs = []
    for obj in objs:
        title=  obj.get("title").replace('\u00a0',' ') # nbsp换成普通空格，看起来一样但可防止出错
        link = obj.get("url")
        author = obj.get("author")
        songs.append([title,author,link])
    return songs
def _redirect_addr(addr:str):
    if addr.find("music.163") < 0:
        return addr # 仅有网易会重定向?
    response = _http.urlopen(url=addr,method="GET",headers=_head,redirect=False)
    response.release_conn()
    return response.headers.get("Location")
def _is_url_ok(url:str):
    x = urlsplit(url)
    cmp = lambda x:len(x) >= 5 and x.find('.') >= 0
    return cmp(x.path) or cmp(x.query)
def getMusicList(name:str,providor="netease",page=1):
    if providor not in PROVIDORS:
        raise ValueError("提供商必须为{0}之一".format(PROVIDORS))
    global _head
    head = _head.copy()
    web_url = HOST + "/?name={0}&type={1}".format(quote(name),providor)
    data = {"input":name,"filter":"name","type":providor,"page":str(page)} # filter应该是name
    head_json = {
        "Accept":ACCEPT_JSON,
        "Origin":HOST,
        "Referer":web_url,
        "X-Requested-With":"XMLHttpRequest"
    }
    # head.update({"Accept":ACCEPT_WEB}) # 问题1：head是就地更新
    # respo_htm = http.request_encode_url("GET",url=web_url,headers=head) # 问题2：直接请求网页得不到结果
    head.update(head_json)
    respo_json = _http.request("POST",url=HOST,fields=data,headers=head,encode_multipart=False,retries=3) # 问题4：保证Content-type正确
    result = _get_links_from_json(respo_json.data.decode("utf-8-sig")) # 问题3：是respo_json 别打错了

    fp = open("links.tmp","w",encoding="utf-8",newline='\n')
    for data in result:
        if data[2] is not None:
            x = _redirect_addr(data[2])
            if _is_url_ok(x):
                fp.write('\u00a0'.join(data) + '\n')
    fp.close()
    return result
if __name__ == "__main__":
    getMusicList("remake")