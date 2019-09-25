

char *mystradd(char *origin, char *addition){
    int x = mystrlen(origin);                 
    int y = mystrlen(addition);               
    char *new = malloc((x+y)*sizeof(char));  
    for(int i=0; i<x; i++){                         
        new[i] = origin[i];                   
    }
    for(int j=0; j<y; j++){
        new[i++] = addition[j];               
    }
    return new;
}

int mystrfind(char *origin, char *substr){
    int x = mystrlen(origin);                 
    int y = mystrlen(substr);
    int val = 0;
    for(int i=0; i<x; i++){
        if(origin[i] == substr[0]) {
             val = 1;
	     for(int j=1; j<b; j++){
                if(origin[i+j] != substr[j]){
                    val = 0;
                    break;
                }
                else if(!val){
                    val = 1;
                }
            }
            if(val){
                break;
            }
        }
    } 
    return val;
}
