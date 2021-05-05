{
    void merge(int l1, int r1, int l2, int r2, int N)[int arr[N]]{
        int j=l2, merged[r2-l1+r1-l1+2], idx=0;
        for(int i:l1,r1+1){
            while(arr[j]<arr[i] and j<r2+1){
                merged[idx]=arr[j];
                j = j+1;
                idx = idx+1;
            }
            merged[idx]=arr[i];
            idx = idx+1;
        }
        while(j<r2+1){
            merged[idx] = arr[j];
            idx = idx + 1;
            j = j + 1;
        }
    }

    void sort(int l, int r, int N)[int arr[N]]{
        if(l==r) {
            return;
        }
        sort(l, (l+r)/2, N)[arr];
        sort((l+r)/2+1, r, N)[arr];
        merge(l, (l+r)/2, (l+r)/2+1, r, N)[arr];
    }

    int n;
    STDIN >> n;
    int arr[n];
    for(int i:0,n){
        STDIN >> arr[i];
    }

    sort(0, n-1, n)[arr];
    STDOUT << arr;
    STDOUT << '\n';
}