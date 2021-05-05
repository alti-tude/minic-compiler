{

    void sort(int N)[int arr[N]]{
        void swap()[int a, int b]{
            int t = a;
            a = b;
            b = t;
        }
        
        for(int i : 0,N){
            for(int j : N,0,-1){
                if(arr[j-1]>arr[j]){
                    swap()[arr[j-1], arr[j]];
                }
            }            
        }
    }
    int n;
    STDIN >> n;
    int arr[n];
    for(int i:0,n){
        STDIN >> arr[i];
    }

    sort(n)[arr];
    STDOUT << arr;
    STDOUT << '\n';
}