
#include<bits/stdc++.h>
#include<chrono>
#include<unistd.h>
#include<sys/types.h>
#include<pthread.h>
#include<semaphore.h>



using namespace std::chrono;
using namespace std;

class func {

    public :
    sem_t data_store;
    
    int unique_process_id;
    string path;
    vector<string> keys;


func(void){
    
    

     sem_init(&data_store,0,1);
     unique_process_id = getpid();
      path = std::to_string(unique_process_id) + "_key_value_store.txt";

}

func(std::string directory){

        sem_init(&data_store,0,1);
        unique_process_id = getpid();
        path = directory + path = std::to_string(unique_process_id) + "_key_value_store.txt";
    }



void Create(std::string key,std::string value,int timetolive){

       sem_wait(&data_store);
       
        if(std::find(keys.begin(),keys.end(),key) != keys.end()){
            std::cout<<"**ERROR**"<<std::endl<<"key already present"<<std::endl;
            sem_post(&data_store);
            return;
        }
std::ifstream read(path,std::ios::binary);
        read.seekg(0,std::ios::end);
        if(read.tellg() >= 1024*1024){
            
            std::cout<<"*ERROR**"<<std::endl<<"exceeded maximum storage capacity"<<std::endl;
            sem_post(&data_store);
            return;
        }

        std::ofstream write;
        write.open(path,ios::app);

        // To enable the time to live functionality, we use the system clock to mark every key's time of creation

        auto current_time = std::chrono::system_clock::now();
        current_time = time_point_cast<std::chrono::seconds>(current_time);
        auto dur = current_time.time_since_epoch();
        dur = duration_cast<seconds>(dur);
        
        
        //string data inserted into file
        std::string text = '"'+key +'"'+ ":" + value + "            " + std::to_string(timetolive) ;

        write<<text<<std::endl;

        write.close();
        
        sem_post(&data_store);

        keys.push_back(key);
        
        return;
    }

void Read(std::string key){

        bool found_key = false;

        if(std::find(keys.begin(),keys.end(),key) == keys.end()){

            std::cout<<"**ERROR**"<<std::endl<<"invalid key"<<std::endl;
            return;
        }
std::ifstream read(path);
        std::string text;
        std::vector<std::string> parsed_text;
        while(read.is_open() && !read.eof()){

            getline(read,text);
            parsed_text = parse(text);

            if(parsed_text[0] == key){

                if(parsed_text[2] != "0") {
                auto current_time = std::chrono::system_clock::now();
                current_time = time_point_cast<std::chrono::seconds>(current_time);
                auto dur = current_time.time_since_epoch();
                dur = duration_cast<seconds>(dur);
                
                long time_lapse,start_time;
                std::stringstream ss;
                ss<<parsed_text[2];
                ss>>time_lapse;
                
                if(time_lapse <= dur.count() - start_time){
                    std::cout<<"**ERROR**"<<std::endl<<"the key has expired"<<std::endl;
                    read.close();
                    sem_post(&data_store);
                    return;
                }
                
                found_key = true;
                std::cout<<parsed_text[1]<<std::endl;

                }
                else {

                    cout<<parsed_text[1]<<endl;
                }

            }
            
        }

        read.close();
        sem_post(&data_store);
        return;

    }
void Delete(std::string key){

        auto find_key  = std::find(keys.begin(),keys.end(),key);

        if(find_key == keys.end()){
            std::cout<<"**ERROR**"<<std::endl<<"invalid key"<<std::endl;
        } 
        keys.erase(find_key);        

        return;
    }


inline std::vector<std::string> parse(std::string text){

        std::vector<std::string> parsed_text;

        int previous = 0;

        for(int i = 0 ; i < text.length();i++){
            if(text[i] == ','){
                parsed_text.push_back(text.substr(previous,i - previous));
                previous = i+1;
            }
        }
        parsed_text.push_back(text.substr(previous,text.length()-previous));

        return parsed_text;

    }
    };
func *thread_example = new func();

void* thread1(void* arg){

    cout<<" Entered thread 1 "<<endl;

    thread_example->Create("thread_example","this is thread safe",0);

    cout<<"Exiting thread 1"<<endl;

}

void* thread2(void *arg){

    cout<<"Entered thread 2"<<endl;

    thread_example->Read("thread_example");

    cout<<"Exiting thread 2"<<endl;
}

int main(){
    
    int choice;
    cout<<"Enter your choice: 1.Test File capacity. 2. Check Time to live feature. 3.Check thread safety. 4.Check deteltion";
    cin>>choice;
    
    if(choice==1){

      //testing file capacity
      func *a = new func();
    
    
    int i =0;
    while(i<25) {
        a->Create(to_string(i),"Test_Cases",0);
        cout<<i<<endl;
        i++;
    }
    }

    
    

  if(choice==2){
   func *b = new func();
   b->Create("first","Test_Case",0);
   b->Read("first");
   b->Create("second","Test_Case",1);
   b->Read("second");

  }  
if((choice==3))
  {
    pthread_t t1,t2;
    pthread_create(&t1,NULL,&thread1,NULL);
    sleep(1);
    pthread_create(&t2,NULL,&thread2,NULL);
    pthread_join(t1,NULL);
    pthread_join(t2,NULL);

  }

if(choice==4)
{
func *c = new func();
    c->Create("Navneel","Pushong",0);
    c->Create("Winston","Churchill",0);
    
    c->Read("Navneel");
    c->Read("Winston");
    
    c->Delete("Winston");
    
    c->Read("Navneel");
    c->Read("Winston");
}
 
    return 0;

}