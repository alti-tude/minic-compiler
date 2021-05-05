{
    int arr[1000];

    void recursion(int index, int N){
        if(index>=N){
            for(int i : 0,N){
                STDOUT << arr[i];
                STDOUT << ' ';
            }
            STDOUT << '\n';
        }

        for(int i : index, N){
            int t = arr[index];
            a = arr[i];
            array[index] = t;
            recursion(index+1, N);
            t = arr[index];
            a = arr[i];
            array[index] = t;
        }
    }

    int main(){
        STDIN >> int N;
        for(int i : 1,N+1){
            arr[i-1] = i;
        }
        recursion(0, N);

    }

}