import sys,search_music,time
from search_music import PROVIDORS
if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise Exception("参数个数不足")
    try:
        i = int(sys.argv[1])
        # st = time.time()
        if len(sys.argv) == 4:
            search_music.getMusicList(sys.argv[2],PROVIDORS[i],page=sys.argv[3])
        else:
            search_music.getMusicList(sys.argv[2],PROVIDORS[i])
        # print("Time={:.3f}s".format(time.time()-st))
    except (ValueError,IndexError) as e:
        raise Exception("参数错误:非法的获取方法") from e