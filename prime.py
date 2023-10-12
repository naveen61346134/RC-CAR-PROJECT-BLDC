import threading
import time


def prime(range_):
    for x in range_:
        if x==2:
            print("Prime 2")
            continue
        for y in range(2,x):
            if x%y==0:
                break
            else:
                if y==(x-1):
                    print("Prime",x)
                    break
                continue


if __name__ == "__main__":
    num_range = int(input("Enter num: "))
    num_2 = int(num_range//2)
    cluster1 = [x for x in range(1,num_2+1)]
    cluster1_half = int(len(cluster1)//2)
    cluster1_subc1 = [y for y in range(1, cluster1_half+1)]
    cluster1_subc2 = [z for z in range(cluster1_half+1, len(cluster1)+1)]
    cluster2 = [v for v in range(num_2+1, num_range+1)]
    cluster2_half = num_range - int(len(cluster2)//2)
    cluster2_subc1 = [w for w in range(len(cluster1)+1, cluster2_half+1)]
    cluster2_subc2 = [q for q in range(cluster2_half+1, num_range+1)]
    print("VALUES CLUSTARISED")
    print("STARTING THREADS")
    time.sleep(0.5)
    
    C1S1T = threading.Thread(target=prime, args=([cluster1_subc1]))
    C1S2T = threading.Thread(target=prime, args=([cluster1_subc2]))
    C2S1T = threading.Thread(target=prime, args=([cluster2_subc1]))
    C2S2T = threading.Thread(target=prime, args=([cluster2_subc2]))
    C1S1T.start()
    C1S2T.start()
    C2S1T.start()
    C2S2T.start()
    C1S1T.join()
    C1S2T.join()
    C2S1T.join()
    C2S2T.join()
