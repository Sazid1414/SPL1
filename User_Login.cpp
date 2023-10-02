#include<bits/stdc++.h>
using namespace std;
int num_of_user;
struct user
{
    char email[100];
    char password[100];
    char name[100];
}user[100];
void register_user()
{
    int i;
    cout<<"Enter the number of users you want to register"<<endl;
    cin>>num_of_user;
    for(i=0;i<num_of_user;i++)
    {
        cout<<"Enter the email of the user"<<endl;
        cin>>user[i].email;
        for(int i=0;i<num_of_user;i++)
        {
            if(strcmp(user[i].email,user[i].email)==0)
            {
                cout<<"Email already exists"<<endl;
                return;
            }
        }
        cout<<"Enter the password of the user"<<endl;
        cin>>user[i].password;
        cout<<"Enter the name of the user"<<endl;
        cin>>user[i].name;
    }
}
void login_user()
{
    char email[100];
    char password[100];
    cout<<"Enter the email of the user"<<endl;
    cin>>email;
    for(int i=0;i<num_of_user;i++)
    {
        if(strcmp(user[i].email,email)==0)
        {
            cout<<"Enter the password of the user"<<endl;
            cin>>password;
            if(strcmp(user[i].password,password)==0)
            {
                cout<<"Login successful"<<endl;
                return;
            }
            else
            {
                cout<<"Login unsuccessful"<<endl;
                return;
            }
        }
    }
}
void FindUser()
{
    fopen("users.txt", "r");
    char email[100];
    char password[100];
    cout<<"Enter the email of the user"<<endl;
    cin>>email;
    for(int i=0;i<num_of_user;i++)
    {
        if(strcmp(user[i].email,email)==0)
        {
            cout<<"Enter the password of the user"<<endl;
            cin>>password;
            if(strcmp(user[i].password,password)==0)
            {
                cout<<"User Found"<<endl;
                return;
            }
            else
            {
                cout<<"This user is not registered !"<<endl;
                return;
            }
        }
    }

}
int main(){
   FILE *file=fopen64("user.txt", "r");
   if(file==NULL)
   {
    cout<<"User not found"<<endl;
    fclose(file);
    return 0;
   }
    else{
        while (fscanf(file, "%s %s %s",user[num_of_user].email,user[num_of_user].password,user[num_of_user].name)!=EOF)
        {
            num_of_user++;
        }
        fclose(file);
    }
    int options;
    do{
        cout<<"1.Register"<<endl;
        cout<<"2.Login"<<endl;
        cout<<"3.Exit"<<endl;
        cin>>options;
        switch(options){
            case 1:
                register_user();
                break;
            case 2:
                login_user();
                break;
            case 3:
                break;
            default:
                cout<<"Invalid option"<<endl;
        }
    }
    while(options!=3);

    return 0;
}