#include <iostream>
#include <cmath>
using namespace std;
///////////////////////////////////////////////////////////////////////////////
const int ASCII = 48;
const int ASCII_TWO = 39;
//parameters of the incoming message:
const int DEST_ADDRESS {4};
const int SRC_ADDRESS {4};
const int SUPPOSED_LENGTH {4};
const int INFORMATION_PART_FRAME = DEST_ADDRESS+SRC_ADDRESS+SUPPOSED_LENGTH;

///////////////////////////////////////////////////////////////////////////////
///// DYNAMIC ARRAY FOR CHAR, 4 CHAR = SOURCE. USED IN SWITCH CLASS  //////////
///////////////////////////////////////////////////////////////////////////////
class Vec_char{
public:
  Vec_char();
  ~Vec_char();
  void add(const unsigned char vec[]); //no other function than add extra source
  bool find(const unsigned char vec[]); //check if the source is present in this array
private:
  int size;
  unsigned char* point;
};

Vec_char::Vec_char(){
  size = 0;
  point = nullptr;
}
Vec_char::~Vec_char(){
  delete [] point;
}

void Vec_char::add(const unsigned char vec[]){
  unsigned char* pnew = nullptr;
  int index = size + SRC_ADDRESS;
  try{
    pnew = new unsigned char[index];
  }catch (bad_alloc& ba){
    cout<<"Bad allocation caught: "<<ba.what()<<endl;
  }
  if(size<0){
    for(int i=0; i<SRC_ADDRESS; ++i){
      pnew[i] = vec[i];
    }
  }else{
    for(int i=0; i<size; ++i){
      pnew[i] = point [i];
    }
    for(int i=size; i<(size+SRC_ADDRESS); ++i){
      pnew[i] = vec[i-size];
    }
  }
  delete [] point;
  point = pnew;
  size = index;
};

bool Vec_char::find(const unsigned char vec[]){
  bool found;
  for(int j=0; j<(size/4); j++){
    for(int i=0+(j*4); i<SRC_ADDRESS+(j*4); i++){
      found = true;
      if(point[i]!=vec[i-(j*4)]){found = false; break;};
    }
    if(found){return true;}
  }
  return false;
}

///////////////////////////////////////////////////////////////////////////////
////////////////// SWITCH: WORKING TOGETHER WITH Vec_char     /////////////////
///////////////////////////////////////////////////////////////////////////////
class Switch{
public:
  Switch();
  ~Switch();
  void create_a_new(const int& links); //recreate the object after aquiring the amount of links
  void backwards_learning(const unsigned char* hex, const int& link); //backwards learning links to sources
  void cast(const unsigned char* hex, const int& link); //broadcast / or specific cast
private:
  int links_available; //holds how many active links are on the switch
  Vec_char* p_sources; //array pointer to objects
};

Switch::Switch(){
  links_available = 0;
  p_sources = nullptr;
}
Switch::~Switch(){
  delete [] p_sources;
}

void Switch::create_a_new(const int& links){
  Vec_char* pnew = nullptr;
  try{
    pnew = new Vec_char[links];
  }catch (bad_alloc& ba){
    cout<<"Bad allocation caught, not that many links possible"<<endl;
  }
  delete [] p_sources;
  links_available = links;
  p_sources = pnew;
}

void Switch::cast(const unsigned char* hex, const int& link){
  bool found;
  for(int i=0; i<links_available; ++i){
    if(p_sources[i].find(hex)){ //check the object (Vec_char) of this place for the dest
      found = true;
      if(i!=link){cout<<" "<<i;} //if dest is same as source
      break;
    }
  }
  if(!found){ //broadcast but not to same source
    //cout<<" Go to the boat Noah! The flood is coming! ";
    for(int i=0; i<links_available; ++i){
      if(i!=link){cout<<" "<<i;}
    }
  }
}

void Switch::backwards_learning(const unsigned char* hex, const int& link){
  bool found;
  found = p_sources[link].find(hex);
  if(!found){p_sources[link].add(hex);}
}

///////////////////////////////////////////////////////////////////////////////
//////////////             NO CLASS FUNCTIONS              ////////////////////
///////////////////////////////////////////////////////////////////////////////
int get_length_of_message(const unsigned char hex[], const int &length_of_arr){
  int temp;
  int length_of_message {0};
  int position = length_of_arr-1;
  const int hex_power {16};

  for(int i=0; i<length_of_arr;++i){
    if(ASCII+10<hex[i]){ //48+10 = 9 in the ascii table
      temp=hex[i] - ASCII-ASCII_TWO; //in this case we are at a-f, so subtract 48+39 to get decimal
    }
    else{
      temp=hex[i]-ASCII; //subtract 48 to get decimal value
    }
    temp = pow(hex_power, position)*temp;
    length_of_message += temp;
    --position;
  }
  cout<<endl;
  length_of_message = length_of_message*2;
  return length_of_message; //return decimal value
}

///////////////////////////////////////////////////////////////////////////////
///////////////////         START OF MAIN            //////////////////////////
///////////////////////////////////////////////////////////////////////////////
int main(){
  Switch azure;
  int how_many_links, how_many_messages, incoming_link;
  int counter;
  unsigned char hex_dest[4]; //holds the destinatoin address
  unsigned char hex_source[4]; //holds the source address
  unsigned char hex_length[4]; //holds the length of the incoming message

  cin>>how_many_links;
  azure.create_a_new(how_many_links); //recreate the array to the amount of links
  cin>>how_many_messages;

  counter = 0;
  while(counter<how_many_messages){
    cin>>incoming_link;

    //start buffering the input and check corresponding destination
    for(int i=0; i<DEST_ADDRESS; ++i){
      cin>>hex_dest[i]; //input to frame for 4 characters
    }
    //continue buffering to get length
    for(int i=0; i<SRC_ADDRESS; ++i){
      cin>>hex_source[i];
    }

    azure.backwards_learning(hex_source, incoming_link);

    //continue buffering to get length
    for(int i=0; i<SUPPOSED_LENGTH; ++i){
      cin>>hex_length[i];
    }
    //create an decimal array for the length
    int length_message = get_length_of_message(hex_length, SUPPOSED_LENGTH);
    unsigned char hex_message[length_message];
    for(int i=0; i<(length_message-INFORMATION_PART_FRAME); ++i){
      cin>>hex_message[i];
    }

    //print out the original message
    for(int i=0; i<DEST_ADDRESS; ++i){
      cout<<hex_dest[i];
    }
    for(int i=0; i<SRC_ADDRESS; ++i){
      cout<<hex_source[i];
    }
    for(int i=0; i<SUPPOSED_LENGTH; ++i){
      cout<<hex_length[i];
    }
    for(int i=0; i<(length_message-INFORMATION_PART_FRAME); ++i){
      cout<<hex_message[i];
    }

    //print out the broadcasted links
    azure.cast(hex_dest, incoming_link);
    ++counter;
  }
  return 0;
}
