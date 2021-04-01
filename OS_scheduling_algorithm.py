
def FCFS_Sum(arr):
    sum=0
    for i in range(1,len(arr)):
        print("the {}th round is from {} to {} = {}".format(i, arr[i-1], arr[i] ,abs(arr[i]-arr[i-1])))
        sum=sum+abs(arr[i]-arr[i-1])
    return sum


def SSTF_Sum(arr):
    sum=0
    used=[]
    for i in range(len(arr)):
      used.append(False)
    current=arr[0]
    used[0]=True
    for i in range(0,len(arr)-1):

        Min=10000

        Next=-1
        for j in range(0,len(arr)):
          if used[j] is False:
           diff=abs(arr[j]-current)
           if diff<Min and diff!=0:
              Min=diff
              Next=j
        used[Next]=True
        print("the {}th round is from {} to {} = {}".format(i+1,current,arr[Next],abs(current-arr[Next])))
        sum+=abs(current-arr[Next])
        current=arr[Next]
    return sum

if __name__ == '__main__':

    FCFS_arr=[2150, 2069, 1212, 2296, 2800, 544, 1618, 356, 1523, 4954, 3681] #2150 is the start
    print("the FCFS's result is {}".format(FCFS_Sum(FCFS_arr)))
    print()
    SSTF_arr=[2150,2296,2069, 1212 , 2800, 544, 1618, 356, 1523, 4954, 3681] #2150 is the start
    print("the SSTF's result is {}".format(SSTF_Sum(SSTF_arr)))
