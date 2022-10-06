import sys
if __name__ == "__main__":
    if len(sys.argv) < 3:
        raise Exception("参数个数不足")
    method = sys.argv[1]
    if method == '1':
        import xzmp3
        xzmp3.get_downpage_list(sys.argv[2])
    elif method == '2':
        import gequbao
        gequbao.get_list(sys.argv[2])
    else:
        raise Exception("参数错误:非法的获取方法")