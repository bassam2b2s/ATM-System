#include <iostream>
#include <fstream>
#include <string>
#include <vector>

using namespace std;

void ClearScreen();
void ShowATMMainMenuScreen();
void ShowQuickWithdrawMenuScreen();
void ShowNormalWithdrawScreen();
void Login();

const string ClientsFile = "Clients.txt";

enum enATMMainMenuOptions { eQuickWithdraw = 1, eNormalWithdraw = 2, eDeposit = 3, eCheckBalance = 4, eLogout = 5 };

struct stClient {

    string AccountNumber = "";
    string PINCode = "";
    string Name = "";
    string Phone = "";
    double AccountBalance;
    bool MarkForDelete = false;

};

stClient CurrentClient;

vector <string> SplitString(string str, string Space) {


    vector <string> vWords = {};
    string Word = "";
    short Position = 0;

    while ((Position = str.find(Space)) != std::string::npos) {

        Word = str.substr(0, Position);

        if (Word != "") {

            vWords.push_back(Word);

        }

        str = str.erase(0, Position + Space.length());

    }

    if (str != "") {

        vWords.push_back(str);

    }

    return vWords;

}

string ConvertClientRecordToLine(stClient Client, string Space) {

    string Line = "";

    Line += Client.AccountNumber + Space;
    Line += Client.PINCode + Space;
    Line += Client.Name + Space;
    Line += Client.Phone + Space;
    Line += to_string(Client.AccountBalance);

    return Line;

}

stClient ConvertLineToClientRecord(string str, string Space) {

    stClient Client;
    vector <string> vWords = SplitString(str, Space);

    Client.AccountNumber = vWords.at(0);
    Client.PINCode = vWords.at(1);
    Client.Name = vWords.at(2);
    Client.Phone = vWords.at(3);
    Client.AccountBalance = stof(vWords.at(4));

    return Client;

}

vector <stClient> LoadClientsDataFromFile(string FileName) {

    fstream MyFile;

    MyFile.open(FileName, ios::in);

    vector <stClient> vClients = {};

    if (MyFile.is_open()) {

        string Line;
        stClient Client;

        while (getline(MyFile, Line))
        {

            Client = ConvertLineToClientRecord(Line, "//#");
            vClients.push_back(Client);

        }

        MyFile.close();

    }

    return vClients;

}

vector <stClient> SaveClientsDataToFile(string FileName, vector <stClient> vClients) {

    fstream MyFile;

    MyFile.open(FileName, ios::out);

    string Line = "";

    if (MyFile.is_open()) {

        for (stClient& Client : vClients) {

            if (Client.MarkForDelete == false) {

                Line = ConvertClientRecordToLine(Client, "//#");
                MyFile << Line << endl;

            }

        }

        MyFile.close();

    }

    return vClients;

}

void BackToATMMainMenu() {

    cout << "\nPress Any Key To Go Back To ATM Main Menu...";
    system("pause>0");
    ShowATMMainMenuScreen();

}

short GetQuickWithdrawAmount(short Option) {

    short QuickWithdraws[] = { 20, 50, 100, 200, 400, 600, 800, 1000 };

    return QuickWithdraws[Option - 1];


}

bool WithdrawBalanceToClientByAccountNumber(string AccountNumber, short WithdrawAmount, vector <stClient>& vClients) {

    char Choice = ' ';

    cout << "\nAre You Sure You Want To Preform This Transaction ? y/n ? ";
    cin >> Choice;

    if (Choice == 'Y' || Choice == 'y') {

        for (stClient& Client : vClients) {

            if (Client.AccountNumber == AccountNumber) {

                Client.AccountBalance -= WithdrawAmount;
                cout << "\nDone Successfully. " << "New Balance is : " << Client.AccountBalance << "\n";
                SaveClientsDataToFile(ClientsFile, vClients);
                return true;

            }

        }

    }

    return false;

}

bool DepositBalanceToClientByAccountNumber(string AccountNumber, short DepositAmount, vector <stClient>& vClients) {

    return WithdrawBalanceToClientByAccountNumber(AccountNumber, DepositAmount * -1, vClients);

}

void BackToQuickWithdrawMenu() {

    cout << "\nPress Any Key To Go Back To Quick Withdraw Menu Screen...";
    system("pause>0");
    ShowQuickWithdrawMenuScreen();

}

void PreformQuickWithdrawMenuOption(short Option) {

    short WithdrawAmount = GetQuickWithdrawAmount(Option);

    if (Option == 9)
        return;

    if (CurrentClient.AccountBalance < WithdrawAmount) {
        cout << "\nYour Balance Is [" << CurrentClient.AccountBalance << "] You Do Not Have Enough To Do This Transaction.\n";
        BackToQuickWithdrawMenu();
        ShowQuickWithdrawMenuScreen();
        return;
    }

    vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    WithdrawBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawAmount, vClients);
    CurrentClient.AccountBalance -= WithdrawAmount;

}

short ReadQuickWithdrawOption(short From, short To) {

    short Option = 0;

    do {

        cout << "\nChoose What Do You Want To Do [" << From << " To " << To << "] : ";
        cin >> Option;

    } while (Option < From || Option > To);


    return Option;

}

void ShowQuickWithdrawMenuScreen() {

    ClearScreen();

    cout << "==================================================================\n";
    cout << "\t\t\tQuick Withdraw Screen\n";
    cout << "==================================================================\n";

    cout << "\t[1] 20\t";
    cout << "\t[2] 50\n";
    cout << "\t[3] 100\t";
    cout << "\t[4] 200\n";
    cout << "\t[5] 400\t";
    cout << "\t[6] 600\n";
    cout << "\t[7] 800\t";
    cout << "\t[8] 1000\n";
    cout << "\t[9] Exit\n";

    cout << "==================================================================\n";

    cout << "Your Balance is " << CurrentClient.AccountBalance << "\n";

    PreformQuickWithdrawMenuOption(ReadQuickWithdrawOption(1, 9));

}

int ReadWithdrawAmount() {

    int WithdrawAmount = 0;

    do {

        cout << "\nEnter An Amount Multiple of 5's ? ";
        cin >> WithdrawAmount;

    } while (WithdrawAmount % 5 != 0);

    return WithdrawAmount;

}

void BackToShowNormalWithdrawScreen() {

    cout << "\nPress Any Key To Go Back To Normal Withdraw Screen...";
    system("pause>0");
    ShowNormalWithdrawScreen();

}

void PreformNormalWithdrawOption() {

    int WithdrawAmount = ReadWithdrawAmount();

    if (CurrentClient.AccountBalance < WithdrawAmount) {
        cout << "\nYour Balance Is [" << CurrentClient.AccountBalance << "] You Do Not Have Enough To Do This Transaction.\n";
        BackToShowNormalWithdrawScreen();
        return;
    }

    vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);
    WithdrawBalanceToClientByAccountNumber(CurrentClient.AccountNumber, WithdrawAmount, vClients);
    CurrentClient.AccountBalance -= WithdrawAmount;

}

void ShowNormalWithdrawScreen() {

    ClearScreen();

    cout << "===============================================\n";
    cout << "\t\tNormal Withdraw Screen\n";
    cout << "===============================================\n";

    PreformNormalWithdrawOption();

}

int ReadDepositAmount() {

    int DepositAmount = 0;

    do {

        cout << "\nEnter A Positive Deposit Amount ? ";
        cin >> DepositAmount;

    } while (DepositAmount < 0);

    return DepositAmount;

}

void PreformDepositOption() {

    int DepositAmount = ReadDepositAmount();

    vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    DepositBalanceToClientByAccountNumber(CurrentClient.AccountNumber, DepositAmount, vClients);
    CurrentClient.AccountBalance += DepositAmount;

}

void ShowDepositScreen() {

    cout << "===============================================\n";
    cout << "\t\tDeposit Screen\n";
    cout << "===============================================\n";

    PreformDepositOption();

}

void ShowCheckBalanceScreen() {

    cout << "===============================================\n";
    cout << "\t\tCheck Balance Screen\n";
    cout << "===============================================\n";

    cout << "Your Balance is " << CurrentClient.AccountBalance << endl;

}

void PreformATMMainMenuOption(enATMMainMenuOptions Option) {

    switch (Option)
    {

    case eQuickWithdraw:

        ClearScreen();
        ShowQuickWithdrawMenuScreen();
        BackToATMMainMenu();
        break;

    case eNormalWithdraw:

        ClearScreen();
        ShowNormalWithdrawScreen();
        BackToATMMainMenu();
        break;

    case eDeposit:
        ClearScreen();
        ShowDepositScreen();
        BackToATMMainMenu();

        break;

    case eCheckBalance:

        ClearScreen();
        ShowCheckBalanceScreen();
        BackToATMMainMenu();
        break;

    case eLogout:

        ClearScreen();
        Login();
        break;

    }

}

enATMMainMenuOptions ReadATMMainMenuOption(short From, short To) {

    short Option = 0;

    do {

        cout << "\nChoose What Do You Want To Do [" << From << " To " << To << "] : ";
        cin >> Option;

    } while (Option < From || Option > To);


    return enATMMainMenuOptions(Option);

}

void ShowATMMainMenuScreen() {

    system("color 0F");
    ClearScreen();

    cout << "==================================================================\n";
    cout << "\t\t\tATM Main Menu Screen\n";
    cout << "==================================================================\n";

    cout << "\t[1] Quick Withdraw.\n";
    cout << "\t[2] Normal Withdraw.\n";
    cout << "\t[3] Deposit.\n";
    cout << "\t[4] Check Balance.\n";
    cout << "\t[5] Logout.\n";

    cout << "==================================================================\n";

    PreformATMMainMenuOption(ReadATMMainMenuOption(1, 5));

}

bool FindClientByAccountNumberAndPINCode(string AccountNumber, string PINCode, stClient& CurrentClient) {

    vector <stClient> vClients = LoadClientsDataFromFile(ClientsFile);

    for (stClient& Client : vClients) {

        if (Client.AccountNumber == AccountNumber && Client.PINCode == PINCode) {

            CurrentClient = Client;
            return true;

        }

    }

    return false;

}

bool LoadClientInfo(string AccountNumber, string PINCode) {

    if (FindClientByAccountNumberAndPINCode(AccountNumber, PINCode, CurrentClient))

        return true;

    else

        return false;

}

string ReadPINCode() {

    string PINCode = "";

    cout << "Enter PIN Code ? ";
    cin >> PINCode;

    return PINCode;

}

string ReadAccountNumber() {

    string AccountNumber = "";

    cout << "Enter Account Number ? ";
    cin >> AccountNumber;

    return AccountNumber;

}

void ClearScreen() {

    system("cls");

}

void ShowLoginScreen() {

    cout << "--------------------------------------\n";
    cout << "\t Login Screen\n";
    cout << "--------------------------------------\n";

}

void Login() {

    string AccountNumber = "";
    string PINCode = "";

    bool LoginFaild = false;

    do {

        ClearScreen();
        ShowLoginScreen();

        if (LoginFaild) {

            cout << "\a";
            system("color 4F");
            cout << "Invalid Account Number/PIN Code!\n";

        }

        AccountNumber = ReadAccountNumber();
        PINCode = ReadPINCode();

        LoginFaild = !(LoadClientInfo(AccountNumber, PINCode));

    } while (LoginFaild);

    ShowATMMainMenuScreen();

}

int main()
{

    Login();

    return 0;
}

