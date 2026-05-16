#include <iostream>
#include <string>
#include <iomanip>
#include <ctime>
#include <conio.h> 
#include <windows.h>
#include <sql.h>
#include <sqlext.h>
using namespace std;



SQLHENV  gEnv  = SQL_NULL_HENV;
SQLHDBC  gDbc  = SQL_NULL_HDBC;

void showSqlError(SQLSMALLINT handleType, SQLHANDLE handle, const char* msg)
{
    SQLCHAR sqlState[6], message[SQL_MAX_MESSAGE_LENGTH];
    SQLINTEGER nativeError;
    SQLSMALLINT textLength;
    if (SQLGetDiagRec(handleType, handle, 1, sqlState, &nativeError,
                      message, sizeof(message), &textLength) == SQL_SUCCESS) {
        cerr << msg << " SQLState=" << sqlState
             << " Message=" << message << endl;
    } else {
        cerr << msg << endl;
    }
}

bool connectDatabase()
{
    SQLRETURN ret;

    // Environment
    ret = SQLAllocHandle(SQL_HANDLE_ENV, SQL_NULL_HANDLE, &gEnv);
    if (!SQL_SUCCEEDED(ret)) return false;
    SQLSetEnvAttr(gEnv, SQL_ATTR_ODBC_VERSION, (void*)SQL_OV_ODBC3, 0);

    // Connection
    ret = SQLAllocHandle(SQL_HANDLE_DBC, gEnv, &gDbc);
    if (!SQL_SUCCEEDED(ret)) {
        showSqlError(SQL_HANDLE_ENV, gEnv, "Alloc DBC failed");
        SQLFreeHandle(SQL_HANDLE_ENV, gEnv);
        gEnv = SQL_NULL_HENV;
        return false;
    }

    SQLCHAR connStr[] =
        "DRIVER={ODBC Driver 18 for SQL Server};"
        "SERVER=ELITEBOOK840G7\\SQLEXPRESS;"       // << NOTE: double backslash
        "DATABASE=BookingManagementSystemm;"
        "Trusted_Connection=Yes;"
        "Encrypt=No;"
        "TrustServerCertificate=Yes;";

    ret = SQLDriverConnect(
        gDbc,
        NULL,
        connStr,
        SQL_NTS,
        NULL,
        0,
        NULL,
        SQL_DRIVER_NOPROMPT
    );

    if (!SQL_SUCCEEDED(ret)) {
        showSqlError(SQL_HANDLE_DBC, gDbc, "SQLDriverConnect failed");
        SQLFreeHandle(SQL_HANDLE_DBC, gDbc);
        SQLFreeHandle(SQL_HANDLE_ENV, gEnv);
        gDbc = SQL_NULL_HDBC;
        gEnv = SQL_NULL_HENV;
        return false;
    }

    return true;
}


void testQuery()
{
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) != SQL_SUCCESS) {
        cout << "Alloc STMT failed\n";
        return;
    }

    SQLRETURN ret = SQLExecDirect(
        hStmt,
        (SQLCHAR*)"SELECT TOP 3 UserId, Name FROM Users",
        SQL_NTS
    );
    if (!SQL_SUCCEEDED(ret)) {
        showSqlError(SQL_HANDLE_STMT, hStmt, "Test SELECT failed");
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return;
    }

    int id;
    char name[101];
    SQLLEN idInd, nameInd;
    SQLBindCol(hStmt, 1, SQL_C_SLONG, &id, 0, &idInd);
    SQLBindCol(hStmt, 2, SQL_C_CHAR,  name, sizeof(name), &nameInd);

    cout << "Users from DB:\n";
    while (SQLFetch(hStmt) == SQL_SUCCESS) {
        cout << "  " << id << " - " << name << "\n";
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
}

void disconnectDatabase()
{
    if (gDbc != SQL_NULL_HDBC) {
        SQLDisconnect(gDbc);
        SQLFreeHandle(SQL_HANDLE_DBC, gDbc);
        gDbc = SQL_NULL_HDBC;
    }
    if (gEnv != SQL_NULL_HENV) {
        SQLFreeHandle(SQL_HANDLE_ENV, gEnv);
        gEnv = SQL_NULL_HENV;
    }
}

bool execNonQuery(const string& sql)
{
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) != SQL_SUCCESS)
        return false;

    SQLRETURN ret = SQLExecDirect(hStmt, (SQLCHAR*)sql.c_str(), SQL_NTS);
    if (!SQL_SUCCEEDED(ret)) {
        showSqlError(SQL_HANDLE_STMT, hStmt, "SQLExecDirect failed");
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return false;
    }

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return true;
}

int getMaxBookingId()
{
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) != SQL_SUCCESS)
        return 1000;

    SQLRETURN ret = SQLExecDirect(
        hStmt,
        (SQLCHAR*)"SELECT ISNULL(MAX(BookingId), 1000) FROM Bookings",
        SQL_NTS
    );
    if (!SQL_SUCCEEDED(ret)) {
        showSqlError(SQL_HANDLE_STMT, hStmt, "SELECT MAX(BookingId) failed");
        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        return 1000;
    }

    int maxId = 1000;
    SQLLEN ind;
    SQLBindCol(hStmt, 1, SQL_C_SLONG, &maxId, 0, &ind);
    if (SQLFetch(hStmt) != SQL_SUCCESS)
        maxId = 1000;

    SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    return maxId;
}




void setColor(int color)
{ 
    SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), color); 
}

// To clear previous input.
void clear() 
{ 
    system("cls"); 
}

void sleep(int ms) 
{ 
    Sleep(ms); 
}

// To stay on the same page until or unless we press any key.
void pressAnyKey()
{
    cout << "\nPress ENTER to continue...";
    cin.ignore(10000, '\n');
    cin.get();
}

void printSlow(string text, int speed = 5)
{
    for(char c : text)
    {
        cout << c;
        Sleep(speed); 
    }
}


const string ADMIN_PASS = "123";  // Admin Password.

// Service Structyre.
struct Service 
{
    int id;
    string name, from, to, dateTime;
    int totalSeats, bookedSeats;
    double price;
    Service *next, *prev;

    Service(int i, string n, string f, string t, string dt, int ts, double p) 
	{
        id = i;
		name = n; 
		from = f; 
		to = t; 
		dateTime = dt;
        totalSeats = ts; 
		bookedSeats = 0; 
		price = p;
        next = prev = nullptr;
    }
    virtual void show() 
	{
        cout << left << setw(6) << id << setw(25) << name << setw(15) << from
             << setw(15) << to << setw(18) << dateTime << setw(8) << (totalSeats-bookedSeats)
             << " Rs." << fixed << setprecision(0) << price << endl;
    }
    virtual ~Service() {}
};

// Flight Structyre.
struct Flight : public Service 
{ 
    Flight(int i, string n, string f, string t, string dt, int s, double p) : Service(i,n,f,t,dt,s,p) {} 
    
	void show() override 
	{ 
	    cout << "[FLIGHT] "; Service::show(); 
	} 
};

// Bus Structyre.
struct Bus : public Service 
{ 
    Bus(int i, string n, string f, string t, string dt, int s, double p) : Service(i,n,f,t,dt,s,p) {} 
	
	void show() override 
	{ 
	    cout << "[BUS]    "; Service::show(); 
	} 
};

// Train Structyre.
struct Train : public Service 
{ 
    Train(int i, string n, string f, string t, string dt, int s, double p) : Service(i,n,f,t,dt,s,p) {} 
	
	void show() override 
	{
	    cout << "[TRAIN]  "; Service::show(); 
	} 
};

// Event Structyre.
struct Event : public Service 
{ 
    Event(int i, string n, string loc, string dt, int s, double p) : Service(i,n,loc,"N/A",dt,s,p) {} 
	
	void show() override 
	{ 
	    cout << "[EVENT]  "; Service::show(); 
	} 
};

// Cinema Structyre.
struct Cinema : public Service 
{ 
    Cinema(int i, string n, string loc, string dt, int s, double p) : Service(i,n,loc,"N/A",dt,s,p) {} 
	
	void show() override 
	{ 
	    cout << "[CINEMA] "; Service::show(); 
	} 
};

// Hotel Structyre.
struct Hotel : public Service 
{ 
    Hotel(int i, string n, string loc, string dt, int s, double p) : Service(i,n,loc,"N/A",dt,s,p) {} 
	
	void show() override 
	{ 
	    cout << "[HOTEL]  "; Service::show(); 
    } 
};

// Car Structyre.
struct Car : public Service 
{ 
    Car(int i, string n, string loc, string dt, int s, double p) : Service(i,n,loc,"Return",dt,s,p) {} 
	
	void show() override 
	{ 
	    cout << "[CAR]    "; Service::show(); 
	} 
};

// Booking Stack.
struct Booking 
{
    int bookingID, userID, serviceID;
    string serviceName, status;
    double paidAmount;
    Booking *next;
    Booking(int bid, int uid, int sid, string sn, double amt) 
	{
        bookingID = bid; 
		userID = uid; 
		serviceID = sid;
        serviceName = sn; 
		status = "Confirmed"; 
		paidAmount = amt;
        next = nullptr;
    }
};

// Waiting Queue.
struct Waiting 
{
    int userID;
    string userName;
    int serviceID;
    Waiting *next;
    
    Waiting(int uid, string n, int sid) : userID(uid), userName(n), serviceID(sid), next(nullptr) {}
};

// Service Manager (Doubly Linked List).
class ServiceList 
{
    public:
    Service *head, *tail;
    Waiting *wFront, *wRear;
    string type;
    int serviceCount;

    ServiceList(string t) : type(t), head(nullptr), tail(nullptr), wFront(nullptr), wRear(nullptr), serviceCount(0) {}

    void addService(Service* s) 
	{
        if (head == nullptr) 
		{
            head = tail = s;
            s->next = s->prev = s;
        } 
		else 
		{
            tail->next = s;
            s->prev = tail;
            s->next = head;
            head->prev = s;
            tail = s;
        }
        serviceCount++;
    }

    Service* findService(int id) 
	{
        if (!head)
		{
			return nullptr;
		} 
		
        Service* temp = head;
        do 
		{
            if (temp->id == id)
			{
				return temp;
			} 
            temp = temp->next;
        } 
		while (temp != head);
        return nullptr;
    }

    void display() 
	{
        if (!head) 
		{ 
		    cout << "No " << type << " available!\n"; 
			return; 
		}
		
		cout<<endl;
        cout << "=== " << type << " ===";
        cout<<endl;
        
        cout << left << setw(6) << "ID" << setw(25) << "Name" << setw(15) << "From"
             << setw(15) << "To" << setw(18) << "Date & Time" << setw(8) << "Seats" << "Price"<<endl;
        cout << string(90, '-') << endl;
       
	    Service* temp = head;
        
		do 
		{
            temp->show();
            temp = temp->next;
        } 
		while (temp != head);
    }

   bool book(int id, int seats, double& cost, bool& waitlisted) 
{
    Service* s = findService(id);
    if (!s) 
    { 
        cout << "Service not found!"<<endl; 
        return false; 
    }
    
    int avail = s->totalSeats - s->bookedSeats;
    
    if (seats > avail) 
    {
        waitlisted = true;
        return false;
    }
    
    s->bookedSeats += seats;
    cost = seats * s->price;
    waitlisted = false;

    // NEW: update DB seats
    string sql = "UPDATE Services SET BookedSeats = " +
                 to_string(s->bookedSeats) +
                 " WHERE ServiceId = " + to_string(s->id);
    execNonQuery(sql);

    return true;
}

    void addToWaiting(int uid, string name, int sid) 
{
    Waiting* w = new Waiting(uid, name, sid);
    if (!wRear) 
    {
        wFront = wRear = w;
    }
    else 
    { 
        wRear->next = w; 
        wRear = w; 
    }
    
    cout << name << " added to waiting list!"<<endl;

    // NEW: save in DB
    string sql = "INSERT INTO WaitingList(UserId, UserName, ServiceId) VALUES (" +
                 to_string(uid) + ", '" + name + "', " + to_string(sid) + ")";
    execNonQuery(sql);
}

    void showWaiting() 
	{
        if (!wFront) 
		{ 
		    cout << "No one in waiting list."<<endl; 
			return; 
		}
		
		cout<<endl;
        cout << "=== Waiting List (" << type << ") ==="<<endl;
        
        Waiting* temp = wFront;
        while (temp) 
		{
            cout << "User ID: " << temp->userID << " | " << temp->userName << " | Service ID: " << temp->serviceID << endl;
            temp = temp->next;
        }
    }

    ~ServiceList() 
	{
        if (head) 
		{
            Service* temp = head->next;
            
            while (temp != head) 
			{
                Service* del = temp;
                temp = temp->next;
                delete del;
            }
            delete head;
        }
        
        while (wFront) 
		{
            Waiting* del = wFront;
            wFront = wFront->next;
            delete del;
        }
    }
};



// Booking History (Stack Per User).
class BookingHistory 
{
    public:
    Booking *top;
    int count;

    BookingHistory() : top(nullptr), count(0) {}

    // addBooking Stack (New Booking appears on the top)
    void addBooking(int bid, int uid, int sid, string sname, double amt) 
	{
        Booking* b = new Booking(bid, uid, sid, sname, amt);
        b->next = top;
        top = b;
        count++;
    }

    // Stack Linked List
    void cancelBooking(int bid) 
	{
        Booking *temp = top, *prev = nullptr;
        
		while (temp) 
		{
            if (temp->bookingID == bid) 
			{
                temp->status = "Cancelled";
                string sql = "UPDATE Bookings SET Status = 'Cancelled' WHERE BookingId = " + to_string(bid);
                execNonQuery(sql);
                cout << "Booking #" << bid << " Cancelled Successfully!"<<endl;
                return;
            }
            prev = temp;
            temp = temp->next;
        }
        cout << "Booking not found!"<<endl;
    }

    ~BookingHistory() 
	{
        while (top) 
		{
            Booking* del = top;
            top = top->next;
            delete del;
        }
    }

void showHistory()
{
    if (!top)
    {
    	setColor(11);
        cout << "---- ";
        cout << "MY ";
        setColor(15);
        cout << "BOOKING ";
        setColor(11);
        cout << "HISTORY ";
        cout << "----" << endl;
        setColor(12);
        cout << "\nNo bookings found!" << endl;
        setColor(15);
        cout << "\nPress ENTER to continue...";
        cin.ignore(10000, '\n');
//        cin.get();        
        setColor(15);
		return;
    }

    Booking* flightsList = nullptr;
    Booking* busesList = nullptr;
    Booking* trainsList = nullptr;
    Booking* eventsList = nullptr;
    Booking* cinemaList = nullptr;
    Booking* hotelsList = nullptr;
    Booking* carsList = nullptr;

    Booking* temp = top;
    while (temp)
    {
        Booking* copy = new Booking(temp->bookingID, temp->userID, temp->serviceID,
                                    temp->serviceName, temp->paidAmount);
        copy->status = temp->status;

        if (temp->serviceID >= 100 && temp->serviceID < 200)
        {
        	copy->next = flightsList, flightsList = copy;
		}
        else if (temp->serviceID >= 200 && temp->serviceID < 300)
        {
        	copy->next = busesList, busesList = copy;
        }
        else if (temp->serviceID >= 300 && temp->serviceID < 400)
        {
        	copy->next = trainsList, trainsList = copy;
		}
        else if (temp->serviceID >= 400 && temp->serviceID < 500)
        {
        	copy->next = eventsList, eventsList = copy;
		}
        else if (temp->serviceID >= 500 && temp->serviceID < 600)
        {
        	copy->next = cinemaList, cinemaList = copy;
		}
        else if (temp->serviceID >= 600 && temp->serviceID < 700)
        {
        	copy->next = hotelsList, hotelsList = copy;
		}
        else
        {
        	copy->next = carsList, carsList = copy;
		}
        temp = temp->next;
    }

    auto printCategory = [](Booking* list, string title, string icon)
    {
        if (!list) return;
        cout << "\n" << icon << " " << title << endl;
        cout << string(title.length() + 3 + icon.length(), '=') << endl;
        Booking* t = list;
        while (t)
        {
            cout << "Booking ID: " << setw(6) << t->bookingID
     << " | " << setw(40) << left << t->serviceName
     << " | " << (t->status == "Confirmed" ? "ACTIVE" : "CANCELLED")
     << " | Rs." << fixed << setprecision(0) << t->paidAmount << endl;
            t = t->next;
        }
        cout << endl;
    };

    cout << "\n----------------------------------------------------------" << endl;
    cout << "                    MY BOOKING HISTORY" << endl;
    cout << "-----------------------------------------------------------" << endl;

    printCategory(flightsList, "FLIGHTS", "Airplane");
    printCategory(busesList,   "BUSES", "Bus");
    printCategory(trainsList,  "TRAINS", "Train");
    printCategory(eventsList,  "EVENTS", "Party");
    printCategory(cinemaList,  "CINEMA", "Movie");
    printCategory(hotelsList,  "HOTELS", "Hotel");
    printCategory(carsList,     "CARS", "Car");

    // Clean memory
    auto cleanup = [](Booking*& list)
    {
        while (list)
        {
            Booking* del = list;
            list = list->next;
            delete del;
        }
    };

    cleanup(flightsList);
    cleanup(busesList);
    cleanup(trainsList);
    cleanup(eventsList);
    cleanup(cinemaList);
    cleanup(hotelsList);
    cleanup(carsList);

    cout << "-----------------------------------------------------------" << endl;
    cout << "Press ENTER to go back...";
    cin.ignore(10000, '\n');
//    cin.get();
}
};

struct User
{
    int id;
    string name, cnic, phone;
    
    BookingHistory history;

    User *next, *prev;
    
    User(int i, string n, string c, string p) 
        : id(i), name(n), cnic(c), phone(p), next(nullptr), prev(nullptr) {}
};

// User Manager Class.
class UserManager 
{
    public:
    User *head, *tail;
    int userCount;

    UserManager() : head(nullptr), tail(nullptr), userCount(0) {}

    void registerUser(int id, string name, string cnic, string phone) 
{
    if (findUser(id)) 
    { 
        cout << "User ID already exists!"<<endl; 
        return; 
    }
    
    User* u = new User(id, name, cnic, phone);
    
    if (!head) 
    {
        head = tail = u;
        u->next = u->prev = u;
    } 
    else 
    {
        tail->next = u;
        u->prev = tail;
        u->next = head;
        head->prev = u;
        tail = u;
    }
    userCount++;
    cout << "Registration Successful! Welcome " << name << endl;

    // NEW: save to DB
    string sql = "INSERT INTO Users(UserId, Name, Cnic, Phone) VALUES (" +
                 to_string(id) + ", '" + name + "', '" + cnic + "', '" + phone + "')";
    execNonQuery(sql);
}

    User* findUser(int id) 
	{
        if (!head) return nullptr;
        User* temp = head;
        do 
		{
            if (temp->id == id) return temp;
            temp = temp->next;
        } 
		while (temp != head);
        return nullptr;
    }

    void showAllUsers()
    {
        system("cls");

        setColor(11);
        cout << "\n------------------------------------------------------------------\n";
        setColor(15);
        cout << "                      ALL REGISTERED USERS\n";
        setColor(11);
        cout << "------------------------------------------------------------------\n\n";
        setColor(7);

        if (!head)
        {
        setColor(8);
        cout << "No users registered yet.\n\n";
        setColor(7);
        cout << "Press ENTER to go back...";
        cin.get();
        return;
        }

    setColor(14);
    cout << left
         << setw(8)  << "User ID"
         << setw(25) << "Name"
         << setw(20) << "CNIC"
         << setw(15) << "Phone"
         << endl;
    setColor(11);
    cout << "------------------------------------------------------------------" << endl;
    setColor(15);

    User* temp = head;
    int count = 0;
    do
    {
        count++;
        cout << left
             << setw(8)  << temp->id
             << setw(25) << temp->name
             << setw(20) << temp->cnic
             << setw(15) << temp->phone << endl;

        temp = temp->next;
    } while (temp != head);

    setColor(11);
    cout << "------------------------------------------------------------------" << endl;
    setColor(10);
    cout << "Total Users: " << count << endl << endl;
    setColor(7);

    cout << "Press ENTER to go back to Admin Panel...";
    cin.ignore(10000, '\n');
}

    ~UserManager() 
	{
        if (head) 
		{
            User* temp = head->next;
            while (temp != head) 
			{
                User* del = temp;
                temp = temp->next;
                delete del;
            }
            delete head;
        }
    }
};

// Main Booking System Class.
class BookmeSystem 
{
    private:
    UserManager users;
    ServiceList flights, buses, trains, events, cinema, hotels, cars;
    BookingHistory* currentUserHistory;
    int bookingCounter;
    
    double totalRevenue;     
    double vendorPayment;    
    double ourProfit;        
    double commissionRate;   

    ServiceList* getCategory(int choice) 
	{
        switch(choice) 
		{
            case 1:
			{
				return &flights;
			}
            case 2: 
			{
			    return &buses;
		    }
            case 3:
			{
			    return &trains;	
			} 
            case 4: 
			{
			    return &events;	
			}
            case 5: 
			{
			    return &cinema;
		    }
            case 6: 
			{
			    return &hotels;	
			}
            case 7: 
			{
				return &cars;
			}
            default: return nullptr;
        }
    }

    void silentlyRegister(int id, string name, string cnic, string phone)
    {
        if (users.findUser(id)) return;
        
        User* u = new User(id, name, cnic, phone);
        
        if (!users.head)
        {
            users.head = users.tail = u;
            u->next = u->prev = u;
        }
        else
        {
            users.tail->next = u;
            u->prev = users.tail;
            u->next = users.head;
            users.head->prev = u;
            users.tail = u;
        }
        users.userCount++;
    }
     
    public:
    BookmeSystem() 
    : flights("Flights"), buses("Buses"), trains("Trains"), events("Events"),
      cinema("Cinema"), hotels("Hotels"), cars("Car Rental"),
      currentUserHistory(nullptr), bookingCounter(1000),
      totalRevenue(0.0), vendorPayment(0.0), ourProfit(0.0), commissionRate(0.15)
    {
    // Pre-Registered Users.
//    silentlyRegister(101, "Saad Akhtar",       "42201-1234567-1", "03001234567");
//    silentlyRegister(102, "Usman Pamper",     "42201-9876543-2", "03219876543");
//    silentlyRegister(103, "Rehman Odd",    "35201-5556667-3", "03331234567");
    
//    loadSampleData();
    }
    
    void loadFromDatabase()
{
    // 1) Load users
    {
        SQLHSTMT hStmt;
        if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) == SQL_SUCCESS) {
            SQLExecDirect(hStmt,
                (SQLCHAR*)"SELECT UserId, Name, Cnic, Phone FROM Users",
                SQL_NTS);

            int id;
            char name[101], cnic[31], phone[31];
            SQLLEN idInd, nInd, cInd, pInd;

            SQLBindCol(hStmt, 1, SQL_C_SLONG, &id, 0, &idInd);
            SQLBindCol(hStmt, 2, SQL_C_CHAR,  name,  sizeof(name),  &nInd);
            SQLBindCol(hStmt, 3, SQL_C_CHAR,  cnic,  sizeof(cnic),  &cInd);
            SQLBindCol(hStmt, 4, SQL_C_CHAR,  phone, sizeof(phone), &pInd);

            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                silentlyRegister(id, name, cnic, phone);
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
    }

    // 2) Load services
    {
        SQLHSTMT hStmt;
        if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) == SQL_SUCCESS) {
            SQLExecDirect(hStmt,
                (SQLCHAR*)"SELECT ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price "
                          "FROM Services ORDER BY ServiceId",
                SQL_NTS);

            int id, totalSeats, bookedSeats;
            char category[21], name[101], from[101], to[101], dt[31];
            double price;
            SQLLEN ind[9];

            SQLBindCol(hStmt, 1, SQL_C_SLONG, &id,         0,           &ind[0]);
            SQLBindCol(hStmt, 2, SQL_C_CHAR,  category,    sizeof(category), &ind[1]);
            SQLBindCol(hStmt, 3, SQL_C_CHAR,  name,        sizeof(name),     &ind[2]);
            SQLBindCol(hStmt, 4, SQL_C_CHAR,  from,        sizeof(from),     &ind[3]);
            SQLBindCol(hStmt, 5, SQL_C_CHAR,  to,          sizeof(to),       &ind[4]);
            SQLBindCol(hStmt, 6, SQL_C_CHAR,  dt,          sizeof(dt),       &ind[5]);
            SQLBindCol(hStmt, 7, SQL_C_SLONG, &totalSeats, 0,           &ind[6]);
            SQLBindCol(hStmt, 8, SQL_C_SLONG, &bookedSeats,0,           &ind[7]);
            SQLBindCol(hStmt, 9, SQL_C_DOUBLE,&price,      0,           &ind[8]);

            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                string cat = category;
                Service* s = nullptr;

                if (cat == "Flight")
                    s = new Flight(id, name, from, to, dt, totalSeats, price);
                else if (cat == "Bus")
                    s = new Bus(id, name, from, to, dt, totalSeats, price);
                else if (cat == "Train")
                    s = new Train(id, name, from, to, dt, totalSeats, price);
                else if (cat == "Event")
                    s = new Event(id, name, from, dt, totalSeats, price);
                else if (cat == "Cinema")
                    s = new Cinema(id, name, from, dt, totalSeats, price);
                else if (cat == "Hotel")
                    s = new Hotel(id, name, from, dt, totalSeats, price);
                else if (cat == "Car")
                    s = new Car(id, name, from, dt, totalSeats, price);

                if (!s) continue;

                s->bookedSeats = bookedSeats;  // keep DB value

                if (id >= 100 && id < 200)      flights.addService(s);
                else if (id >= 200 && id < 300) buses.addService(s);
                else if (id >= 300 && id < 400) trains.addService(s);
                else if (id >= 400 && id < 500) events.addService(s);
                else if (id >= 500 && id < 600) cinema.addService(s);
                else if (id >= 600 && id < 700) hotels.addService(s);
                else if (id >= 700 && id < 800) cars.addService(s);
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
    }

    // 3) Set bookingCounter = max BookingId and load bookings into each user's history
    bookingCounter = getMaxBookingId();

    {
        SQLHSTMT hStmt;
        if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) == SQL_SUCCESS) {
            SQLExecDirect(hStmt,
                (SQLCHAR*)"SELECT BookingId, UserId, ServiceId, ServiceName, Status, PaidAmount "
                          "FROM Bookings",
                SQL_NTS);

            int bid, uid, sid;
            char sname[101], status[21];
            double paid;
            SQLLEN ind[6];

            SQLBindCol(hStmt, 1, SQL_C_SLONG, &bid,   0,        &ind[0]);
            SQLBindCol(hStmt, 2, SQL_C_SLONG, &uid,   0,        &ind[1]);
            SQLBindCol(hStmt, 3, SQL_C_SLONG, &sid,   0,        &ind[2]);
            SQLBindCol(hStmt, 4, SQL_C_CHAR,  sname, sizeof(sname), &ind[3]);
            SQLBindCol(hStmt, 5, SQL_C_CHAR,  status,sizeof(status),&ind[4]);
            SQLBindCol(hStmt, 6, SQL_C_DOUBLE,&paid, 0,        &ind[5]);

            while (SQLFetch(hStmt) == SQL_SUCCESS) {
                User* u = users.findUser(uid);
                if (!u) continue;

                u->history.addBooking(bid, uid, sid, sname, paid);
                if (u->history.top && u->history.top->bookingID == bid) {
                    u->history.top->status = status;
                }

                // Rebuild revenue numbers
                if (string(status) == "Confirmed") {
                    totalRevenue  += paid;
                    vendorPayment += paid * 0.85;
                    ourProfit     += paid * 0.15;
                }
            }

            SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
        }
    }

    // 4) Load waiting list
{
    SQLHSTMT hStmt;
    if (SQLAllocHandle(SQL_HANDLE_STMT, gDbc, &hStmt) == SQL_SUCCESS) {
        SQLExecDirect(hStmt,
            (SQLCHAR*)"SELECT UserId, UserName, ServiceId FROM WaitingList",
            SQL_NTS);

        int uid, sid;
        char uname[101];
        SQLLEN uInd, nInd, sInd;

        SQLBindCol(hStmt, 1, SQL_C_SLONG, &uid, 0, &uInd);
        SQLBindCol(hStmt, 2, SQL_C_CHAR,  uname, sizeof(uname), &nInd);
        SQLBindCol(hStmt, 3, SQL_C_SLONG, &sid, 0, &sInd);

        while (SQLFetch(hStmt) == SQL_SUCCESS) {
            ServiceList* list = nullptr;
            if      (sid >= 100 && sid < 200) list = &flights;
            else if (sid >= 200 && sid < 300) list = &buses;
            else if (sid >= 300 && sid < 400) list = &trains;
            else if (sid >= 400 && sid < 500) list = &events;
            else if (sid >= 500 && sid < 600) list = &cinema;
            else if (sid >= 600 && sid < 700) list = &hotels;
            else if (sid >= 700 && sid < 800) list = &cars;

            if (!list) continue;

            Waiting* w = new Waiting(uid, uname, sid);
            if (!list->wRear) {
                list->wFront = list->wRear = w;
            } else {
                list->wRear->next = w;
                list->wRear       = w;
            }
        }

        SQLFreeHandle(SQL_HANDLE_STMT, hStmt);
    }
}
}

    // Already Loaded data.
    void loadSampleData() 
	{
		
        // Flights
        flights.addService (new Flight(101, "PK-308 LHR-KHI", "Lahore",        "Karachi",     "2025-11-27 10:00", 150,  8500));
        flights.addService (new Flight(102, "PK-309 ISB-DXB", "Islamabad",     "Dubai",       "2025-11-28 14:30", 120, 25000));
        flights.addService (new Flight(103, "ER-310 KHI-LHR", "Karachi",       "Lahore",      "2025-11-27 18:00", 140,  8200));
        flights.addService (new Flight(104, "PA-311 LHE-ISB", "Lahore",        "Islamabad",   "2025-11-26 09:15", 100,  4500));
        flights.addService (new Flight(105, "PK-312 ISB-KHI", "Islamabad",     "Karachi",     "2025-11-29 20:00", 130,  9000));

        // Busses.
        buses.addService (new Bus(201, "Daewoo Express LHR-ISB",   "Lahore",     "Islamabad",   "2025-11-26 08:00", 45, 1800));
        buses.addService (new Bus(202, "Faisal Movers  LHR-KHI",   "Lahore",     "Karachi",     "2025-11-27 21:00", 40, 3500));
        buses.addService (new Bus(203, "Skyways        ISB-LHR",   "Islamabad",  "Lahore",      "2025-11-26 12:30", 50, 1700));
        buses.addService (new Bus(204, "Bilal Travels  KHI-LHR",   "Karachi",    "Lahore",      "2025-11-28 19:00", 42, 3600));
        buses.addService (new Bus(205, "Niazi Express  MUL-LHR",   "Multan",     "Lahore",      "2025-11-26 10:00", 48, 1400));

        // Trains
        trains.addService (new Train(301, "Khyber Mail",           "Karachi",    "Peshawar",    "2025-11-27 20:00", 400, 2500));
        trains.addService (new Train(302, "Tezgam Express",        "Karachi",    "Rawalpindi","2025-11-26 17:00", 380, 2800));
        trains.addService (new Train(303, "Karachi Express",       "Lahore",     "Karachi",   "2025-11-28 18:00", 350, 2700));
        trains.addService (new Train(304, "Shalimar Express",      "Lahore",     "Karachi",   "2025-11-29 06:00", 420, 2400));
        trains.addService (new Train(305, "Green Line",            "Islamabad",  "Karachi",   "2025-11-27 15:00", 300, 4500));

        // Events
        events.addService (new Event(401, "PSL Final 2025",               "Gaddafi Stadium",        "2025-12-15 19:00", 50000, 3000));
        events.addService (new Event(402, "Atif Aslam Live Concert",      "Expo Center Lahore",     "2025-12-20 20:00", 10000, 5000));
        events.addService (new Event(403, "Food Festival 2025",           "Lahore Fort",            "2025-12-25 16:00", 15000, 1500));
        events.addService (new Event(404, "Comedy Night - Zakir Khan",    "Alhamra Hall",           "2025-11-30 20:30", 3000,  3500));
        events.addService (new Event(405, "Pakistan vs India - ODI",      "National Stadium Khi",   "2025-12-10 14:00", 45000, 8000));

        // Cinema
        cinema.addService (new Cinema(501, "Deadpool & Wolverine 7PM",   "Cinepax Lahore",    "2025-11-26 19:00", 250, 900));
        cinema.addService (new Cinema(502, "Joker 2 - 9PM Show",         "Universal Cinema", "2025-11-26 21:00", 200, 1000));
        cinema.addService (new Cinema(503, "Animal - 3D 6PM",            "DHA Cinema",       "2025-11-27 18:00", 180, 1200));
        cinema.addService (new Cinema(504, "Nayab - Premiere",           "Cinegold Bahria",  "2025-11-28 20:30", 300, 1500));
        cinema.addService (new Cinema(505, "Inside Out 2 - 4PM Kids",    "Cinepax Packages", "2025-11-26 16:00", 220, 700));

        // Hotels
        hotels.addService (new Hotel(601, "Pearl Continental Lahore", "Lahore",    "2025-11-27", 120, 15000));
        hotels.addService (new Hotel(602, "Avari Hotel Lahore      ", "Lahore",    "2025-11-28", 100, 13000));
        hotels.addService (new Hotel(603, "Serena Hotel Islamabad  ", "Islamabad", "2025-11-27", 150, 18000));
        hotels.addService (new Hotel(604, "PC Bhurban Resort       ", "Murree",    "2025-11-29", 80,  22000));
        hotels.addService (new Hotel(605, "Movenpick Karachi       ", "Karachi",   "2025-11-30", 110, 16000));

        // Cars.
        cars.addService (new Car(701, "Toyota Corolla 2024",    "Lahore Airport",    "2025-11-27", 4, 4000));
        cars.addService (new Car(702, "Honda Civic 2023",       "Islamabad Airport", "2025-11-28", 4, 4500));
        cars.addService (new Car(703, "Suzuki Alto (Budget)",   "Karachi Saddar",    "2025-11-26", 4, 2500));
        cars.addService (new Car(704, "Toyota Prado 4x4",       "Lahore DHA",       "2025-11-29", 6,  12000));
        cars.addService (new Car(705, "KIA Sportage",           "Islamabad F-10",    "2025-11-27", 5, 8000));
    }

void userMenu(int userID)
{
    User* user = users.findUser(userID);
    
    if (!user)
    {
        setColor(12);
        cout << "\nUser not found!\n";
        setColor(7);
        cout << "Press ENTER to continue...";
        cin.ignore(10000, '\n');
        cin.get();
        return;
    }

    int choice;
    do
    {
        system("cls");

        setColor(11);
        cout << "------------ ";
        cout << "WELCOME ";
        setColor(15);
        cout << user->name;
        setColor(11);
        cout << " ------------\n\n";
        setColor(15);

        cout << "1. Flights \n2. Buses \n3. Trains \n4. Events";
        cout << "\n5. Cinema \n6. Hotels \n7. Cars \n8. My Bookings";
        cout << "\n9. Search \n0. Logout ";
        setColor(14);
        cout << "\n\nWhat would you like to book today?\n";
        setColor(11);
        cout << "Enter your choice: ";
        setColor(15);

        choice = -1;

        if (!(cin >> choice))
        {
            setColor(12);
            cout << "\nInvalid input! Please enter a number only.\n";
            setColor(7);
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Press ENTER to continue...";
            cin.get();
            continue;
        }

        cin.ignore(10000, '\n');

        if (choice == 0)
        {
            setColor(14);
            cout << "\nLogging out... Thank you, " << user->name << "!\n";
            setColor(7);
            cout << "Press ENTER to continue...";
            cin.get();
            break;
        }

        if (choice >= 1 && choice <= 7)
        {
            ServiceList* cat = getCategory(choice);
            system("cls");

            setColor(11);
            cout << "------------ ";
            setColor(15);
            cout << cat->type;
            setColor(11);
            cout << " ------------\n\n";
            setColor(7);

            setColor(15);
            cat->display();
            cout << endl;

            int sid, seats = 1;
            cout << "Enter Service ID to book: ";
            if (!(cin >> sid))
            {
                setColor(12);
                cout << "\nInvalid ID!\n";
                setColor(7);
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Press ENTER to continue...";
                cin.get();
                continue;
            }

            Service* s = cat->findService(sid);
            if (!s)
            {
                setColor(12);
                cout << "\nService not found!\n";
                setColor(7);
                cout << "Press ENTER to continue...";
                cin.get();
                continue;
            }

            if (choice != 6 && choice != 7)
            {
                cout << "Number of seats: ";
                if (!(cin >> seats) || seats <= 0)
                {
                    setColor(12);
                    cout << "\nInvalid number of seats!\n";
                    setColor(7);
                    cin.clear();
                    cin.ignore(10000, '\n');
                    cout << "Press ENTER to continue...";
                    cin.get();
                    continue;
                }
            }

            double cost;
            bool wait = false;
            if (cat->book(sid, seats, cost, wait))
{
    string promo;
    cout << "\nPromo Code (or 0): ";
    cin >> promo;
    if (promo == "SAVE10") cost *= 0.9;
    if (promo == "SAVE20") cost *= 0.8;

    cout << "\nTotal: Rs." << fixed << setprecision(2) << cost << endl;
    cout << "Payment: \n1.Card \n2.JazzCash \n3.Easypaisa \nChoice: ";
    int pay; cin >> pay;

    // FIRST: generate the booking ID
    int newBookingId = ++bookingCounter;

    // THEN: print messages using it
    setColor(10);
    cout << "\nPAYMENT SUCCESSFUL!\n";
    cout << "Booking confirmed for " << s->name << "!\n";
    cout << "Your Booking ID is: " << newBookingId << "\n";
    cout << "Enjoy your trip, " << user->name << "!\n";
    setColor(7);

    // Save in history
    user->history.addBooking(newBookingId, userID, sid, s->name, cost);

    // Save in DB (INSERT, not UPDATE)
    string sql =
        "INSERT INTO Bookings (BookingId, UserId, ServiceId, ServiceName, Status, PaidAmount) VALUES (" +
        to_string(newBookingId) + ", " +
        to_string(userID)       + ", " +
        to_string(sid)          + ", '" +
        s->name + "', 'Confirmed', " +
        to_string(cost) + ")";
    execNonQuery(sql);

    double finalPrice = cost;
    totalRevenue  += finalPrice;
    vendorPayment += finalPrice * 0.85;
    ourProfit     += finalPrice * 0.15;
}
            else
            {
                setColor(12);
                cout << "\nNo seats available!\n";
                setColor(7);
                cout << "Add to waiting list? (y/n): ";
                char ch; cin >> ch;
                if (ch == 'y' || ch == 'Y')
                    cat->addToWaiting(userID, user->name, sid);
            }

            cout << "\nPress ENTER to continue...";
            cin.ignore(10000, '\n');
            cin.get();
        }

        else if (choice == 8)
{
    system("cls");

    // If user has no bookings, just show history and return to menu
    if (!user->history.top)
    {
        user->history.showHistory();
        continue;
    }

    // Show full booking history
    user->history.showHistory();

    // Ask if they want to cancel
    char wantCancel;
    cout << "\nDo you want to cancel a booking? (y/n): ";
    cin >> wantCancel;
    cin.ignore(10000, '\n');

    if (wantCancel == 'y' || wantCancel == 'Y')
    {
        int bookingIdToCancel;   // <--- declared here

        cout << "Enter Booking ID to cancel: ";
        if (!(cin >> bookingIdToCancel))
        {
            setColor(12);
            cout << "\nInvalid Booking ID!\n";
            setColor(7);
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Press ENTER to continue...";
            cin.get();
            continue;
        }
        cin.ignore(10000, '\n');

        // Find this booking in the user's history
        Booking* b = user->history.top;
        while (b && b->bookingID != bookingIdToCancel)
            b = b->next;

        if (!b)
        {
            setColor(12);
            cout << "\nBooking ID " << bookingIdToCancel << " not found in your history.\n";
            setColor(7);
        }
        else if (b->status == "Cancelled")
        {
            setColor(12);
            cout << "\nBooking #" << bookingIdToCancel << " is already cancelled.\n";
            setColor(7);
        }
        else
        {
            double original = b->paidAmount;
            double refund   = original * 0.75;   // 75% refund
            double penalty  = original - refund; // 25% cancellation charges

            // Mark as cancelled in memory
            b->status = "Cancelled";

            // Update in database (current DB is BookingManagementSystem)
            string sql = "UPDATE Bookings "
                         "SET Status = 'Cancelled' "
                         "WHERE BookingId = " + to_string(bookingIdToCancel);
            bool ok = execNonQuery(sql);

            if (!ok)
            {
                setColor(12);
                cout << "\nDatabase update failed. Booking status may not be saved.\n";
                setColor(7);
            }
            else
            {
                // Adjust revenue numbers to account for refund
                totalRevenue  -= refund;
                vendorPayment -= refund * 0.85;
                ourProfit     -= refund * 0.15;

                setColor(10);
                cout << "\nBooking #" << bookingIdToCancel << " cancelled successfully.\n";
                setColor(7);
                cout << "Original paid amount : Rs. " << fixed << setprecision(2) << original << "\n";
                cout << "Refund (75%)         : Rs. " << refund  << "\n";
                cout << "Cancellation charges : Rs. " << penalty << "\n";
            }
        }

        cout << "\nPress ENTER to continue...";
        cin.get();
    }
}

        else if (choice == 9)  
{
    system("cls");
    setColor(11);
    cout << "---------------- \n";
    setColor(15);
    cout << "SEARCH SERVICES\n";
    setColor(11);
    cout << "----------------\n\n";
    setColor(15);

    string keyword;
    double minPrice = 0, maxPrice = 999999;

    cout << "Enter keyword (name/city) or press ENTER to skip: ";
    cin.ignore();
    getline(cin, keyword);

    cout << "Minimum price (0 for no limit): ";
    if (!(cin >> minPrice)) minPrice = 0;

    cout << "Maximum price (999999 for no limit): ";
    if (!(cin >> maxPrice)) maxPrice = 999999;

    string keyLower = keyword;
    for (int i = 0; i < keyLower.length(); i++)
        keyLower[i] = tolower(keyLower[i]);

    bool foundAny = false;

    auto searchInList = [&](ServiceList* list, string catName)
    {
        if (!list->head) return;

        Service* temp = list->head;
        do
        {
            string nameLower = temp->name;
            string fromLower = temp->from;
            string toLower   = temp->to;
            for (int i = 0; i < nameLower.length(); i++) nameLower[i] = tolower(nameLower[i]);
            for (int i = 0; i < fromLower.length(); i++) fromLower[i] = tolower(fromLower[i]);
            for (int i = 0; i < toLower.length();   i++) toLower[i]   = tolower(toLower[i]);

            bool matchKeyword = keyword.empty() ||
                nameLower.find(keyLower) != string::npos ||
                fromLower.find(keyLower) != string::npos ||
                toLower.find(keyLower) != string::npos;

            bool matchPrice = temp->price >= minPrice && temp->price <= maxPrice;
            bool hasSeats   = temp->totalSeats > temp->bookedSeats;

            if (matchKeyword && matchPrice && hasSeats)
            {
                if (!foundAny)
                {
                    setColor(14);
                    cout << "\nSEARCH RESULTS:\n";
                    setColor(11);
                    cout << string(70, '=') << endl;
                    setColor(7);
                    cout << left
                         << setw(6)  << "ID"
                         << setw(32) << "Service Name"
                         << setw(14) << "From"
                         << setw(14) << "To"
                         << setw(10) << "Seats"
                         << "Price" << endl;
                    setColor(8);
                    cout << string(85, '-') << endl;
                    setColor(15);
                    foundAny = true;
                }

                cout << left
                     << setw(6)  << temp->id
                     << setw(32) << temp->name
                     << setw(14) << temp->from
                     << setw(14) << temp->to
                     << setw(10) << (temp->totalSeats - temp->bookedSeats)
                     << "Rs." << fixed << setprecision(0) << temp->price << endl;
            }

            temp = temp->next;
        } while (temp != list->head);
    };

    searchInList(&flights, "Flights");
    searchInList(&buses,   "Buses");
    searchInList(&trains,  "Trains");
    searchInList(&events,  "Events");
    searchInList(&cinema,  "Cinema");
    searchInList(&hotels,  "Hotels");
    searchInList(&cars,    "Cars");

    if (!foundAny)
    {
        setColor(12);
        cout << "\nNo services found matching your search!\n\n";
        setColor(7);
    }
    else
    {
        setColor(10);
        cout << "\nSearch completed successfully!\n";
        setColor(7);
    }

    cout << "\nPress ENTER to return to menu...";
    cin.ignore(10000, '\n');
    cin.get();
}

        else
        {
            setColor(12);
            cout << "\nInvalid choice! Please enter 0-9 only.\n";
            setColor(7);
            cout << "Press ENTER to continue...";
            cin.get();
        }

    } while (choice != 0);
}

void adminMenu()
{
    string password;
    int attempts = 3;

    while (attempts > 0)
    {
        cout << endl;
        setColor(11);
        cout << "Enter Admin Password: ";
        cin >> password;
        setColor(7);
        
        if (password == "123")
        {
        	setColor(10);
        	system("cls");
            cout << "\nLogin Successful! Welcome Admin!\n";
            setColor(15);
            pressAnyKey();
            break;
        }
        else
        {
        	setColor(12);
            attempts--;
            cout << "\nWrong Password! " << attempts << " attempt(s) remaining.\n";
            if (attempts == 0)
            {
                cout << "Access Denied! Too many wrong attempts.\n\n";
                setColor(15);
                cout << "Press ENTER to return...";
                cin.ignore(10000, '\n');
                cin.get();
                return;
            }
        }
    }

    int choice;
    while (true)
    {
        system("cls");  // Clean screen
        setColor(11);
        printSlow("============ ", 1);
        setColor(11);
        setColor(15);
        printSlow("ADMIN PANEL ", 1);
        setColor(11);
        printSlow("============\n\n", 1);
        setColor(15);
        cout << "1. Add Service" << endl;
        cout << "2. Edit Service" << endl;
        cout << "3. Delete Service" << endl;
        cout << "4. View All Services" << endl;
        cout << "5. List of Vendors & Their Services" << endl;
        cout << "6. View All Registered Users" << endl;
        cout << "7. Revenue & Profit Report" << endl;
        cout << "8. Booking History" << endl;
        cout << "0. Logout" << endl << endl;
        setColor(11);
        cout << "Choice: ";

        choice = -1;
        if (!(cin >> choice))
        {
        	setColor(12);
            cout << "\nInvalid input! Please enter a number only." << endl;
            cin.clear();
            setColor(15);
            pressAnyKey();
            continue;
        }
        cout<<endl;
        cin.ignore(10000, '\n');
        setColor(7);
        if (choice == 0)
        {
            cout << "\nLogging out..." << endl;
            cout << "Press ENTER to return to main menu...";
            cin.get();
            return;
        }

        if (choice == 1)  
        {
            int cat;
            do
            {
            	system("cls"); 
            setColor(11);
                cout << "---------- ";
                setColor(15);
                cout << "Select Category to Add";
                setColor(11);
                cout << " ----------\n" << endl;
                setColor(15);
                cout << "1. Flight" << endl;
                cout << "2. Bus" << endl;
                cout << "3. Train" << endl;
                cout << "4. Event" << endl;
                cout << "5. Cinema" << endl;
                cout << "6. Hotel" << endl;
                cout << "7. Car Rental" << endl;
                cout << "0. Back to Admin Panel\n" << endl;
                
                setColor(11);
                cout << "Choice: ";

                if (!(cin >> cat))
                {
                	setColor(12);
                    cout << "\nInvalid input! Enter a number." << endl;
                    cin.clear();
                    setColor(15);
                    pressAnyKey();
                    continue;
                }
                
                if (cat == 0) break;

                if (cat < 1 || cat > 7)
                {
                	setColor(12);
                    cout << "Invalid category! Try again." << endl;
                    setColor(15);
                    cout << "Press ENTER to continue...";
                    cin.get();
                    continue;
                }
                setColor(7);
                int newID = 0;
                switch(cat)
                {
                    case 1: newID = 100 + (++flights.serviceCount); break;
                    case 2: newID = 200 + (++buses.serviceCount); break;
                    case 3: newID = 300 + (++trains.serviceCount); break;
                    case 4: newID = 400 + (++events.serviceCount); break;
                    case 5: newID = 500 + (++cinema.serviceCount); break;
                    case 6: newID = 600 + (++hotels.serviceCount); break;
                    case 7: newID = 700 + (++cars.serviceCount); break;
                }

                string name, from, to, dateTime;
                int seats;
                double price;
                
                setColor(15);
                cout << "\nNew Service ID: " << newID << endl;

                cout << "Enter Name (e.g. PK-308 LHR-KHI): ";
                cin.ignore();
                getline(cin, name);

                if (cat == 1 || cat == 2 || cat == 3 || cat == 7)
                {
                    cout << "From Location: ";
                    getline(cin, from);
                    if (cat != 7)
                    {
                        cout << "To Location: ";
                        getline(cin, to);
                    }
                    else
                        to = "Return";
                }
                else
                {
                    from = name;
                    to = "N/A";
                    cout << "Location/Venue: ";
                    getline(cin, from);
                }

                cout << "Date & Time (e.g. 2025-11-30 19:00): ";
                getline(cin, dateTime);

                cout << "Total Seats/Rooms/Cars: ";
                while (!(cin >> seats) || seats <= 0)
                {
                	setColor(12);
                    cout << "\nInvalid! Enter positive number: ";
                    setColor(15);
                    cin.clear();
                    cin.ignore(10000, '\n');
                }
                
                setColor(15);
                cout << "Price (PKR): ";
                while (!(cin >> price) || price < 0)
                {
                	setColor(12);
                    cout << "\nInvalid price! Enter valid amount: ";
                    setColor(15);
                    cin.clear();
                    cin.ignore(10000, '\n');
                }

                ServiceList* selected = nullptr;
                switch(cat)
                {
                    case 1: selected = &flights; break;
                    case 2: selected = &buses; break;
                    case 3: selected = &trains; break;
                    case 4: selected = &events; break;
                    case 5: selected = &cinema; break;
                    case 6: selected = &hotels; break;
                    case 7: selected = &cars; break;
                }

                if (selected)
                {
                    Service* newService = nullptr;
                    if (cat == 1) newService = new Flight(newID, name, from, to, dateTime, seats, price);
                    else if (cat == 2) newService = new Bus(newID, name, from, to, dateTime, seats, price);
                    else if (cat == 3) newService = new Train(newID, name, from, to, dateTime, seats, price);
                    else if (cat == 4) newService = new Event(newID, name, from, dateTime, seats, price);
                    else if (cat == 5) newService = new Cinema(newID, name, from, dateTime, seats, price);
                    else if (cat == 6) newService = new Hotel(newID, name, from, dateTime, seats, price);
                    else if (cat == 7) newService = new Car(newID, name, from, dateTime, seats, price);

                    selected->addService(newService);
                    
                    string categoryName;
switch (cat) {
    case 1: categoryName = "Flight"; break;
    case 2: categoryName = "Bus";    break;
    case 3: categoryName = "Train";  break;
    case 4: categoryName = "Event";  break;
    case 5: categoryName = "Cinema"; break;
    case 6: categoryName = "Hotel";  break;
    case 7: categoryName = "Car";    break;
}

string sql = "INSERT INTO Services(ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES (" +
             to_string(newID) + ", '" + categoryName + "', '" + name + "', '" + from + "', '" + to + "', '" +
             dateTime + "', " + to_string(seats) + ", 0, " + to_string(price) + ")";
execNonQuery(sql);
                    setColor(10);
                    cout << "\nService Added Successfully with ID: " << newID << "!" << endl;
                    setColor(15);
                    pressAnyKey();
                }

            } while (cat != 0);
        }

else if (choice == 2)
{
    system("cls");
    setColor(11);
    cout << "---------- ";
    setColor(15);
    cout << "EDIT SERVICE";
    setColor(11);
    cout << " ----------" << endl << endl;
    setColor(15);

    int searchID;
    cout << "Enter Service ID to edit: ";
    while (!(cin >> searchID))
    {
        setColor(12);
        cout << "\nInvalid ID! Enter numbers only: ";
        setColor(15);
        cin.clear();
        cin.ignore(10000, '\n');
    }

    Service* found = nullptr;
    ServiceList* lists[] = {&flights, &buses, &trains, &events, &cinema, &hotels, &cars};
    string names[] = {"Flight", "Bus", "Train", "Event", "Cinema", "Hotel", "Car"};

    bool serviceFound = false;

    for (int i = 0; i < 7; i++)
    {
        found = lists[i]->findService(searchID);
        if (found)
        {
            serviceFound = true;
            setColor(10);
            cout << "\nService Found in " << names[i] << " Category!" << endl;
            setColor(7);
            found->show();
            cout << endl;

            setColor(11);
            cout << "=== ENTER NEW DETAILS ===" << endl;
            setColor(15);

            cout << "New Name: ";
            cin.ignore();
            getline(cin, found->name);

            if (i <= 2 || i == 6)
            {
                cout << "New From Location: ";
                getline(cin, found->from);
                if (i != 6)
                {
                    cout << "New To Location: ";
                    getline(cin, found->to);
                }
                else
                    found->to = "Return";
            }
            else
            {
                cout << "New Location/Venue: ";
                getline(cin, found->from);
                found->to = "N/A";
            }

            cout << "New Date & Time: ";
            getline(cin, found->dateTime);

            cout << "New Total Seats/Rooms/Cars: ";
            while (!(cin >> found->totalSeats) || found->totalSeats <= 0)
            {
                setColor(12);
                cout << "\nInvalid! Enter positive number: ";
                setColor(15);
                cin.clear();
                cin.ignore(10000, '\n');
            }

            cout << "New Price (PKR): ";
            while (!(cin >> found->price) || found->price < 0)
            {
                setColor(12);
                cout << "\nInvalid price! Enter valid amount: ";
                setColor(15);
                cin.clear();
                cin.ignore(10000, '\n');
            }
            
            string sql = "UPDATE Services SET "
             "Name = '" + found->name + "', "
             "FromLoc = '" + found->from + "', "
             "ToLoc = '" + found->to + "', "
             "DateTime = '" + found->dateTime + "', "
             "TotalSeats = " + to_string(found->totalSeats) + ", "
             "Price = " + to_string(found->price) +
             " WHERE ServiceId = " + to_string(found->id);
execNonQuery(sql);

            setColor(10);
            cout << "\nSERVICE UPDATED SUCCESSFULLY!" << endl;
            setColor(7);
            found->show();
            setColor(15);
            pressAnyKey();
            break;  
        }
    }

    if (!serviceFound)
    {
        setColor(12);
        cout << "\nService ID " << searchID << " not found!" << endl;
        setColor(15);
        pressAnyKey();
    }

}

        else if (choice == 3) 
        {
            system("cls");
            setColor(11);
            cout << "---------- ";
            setColor(15);
            cout << "DELETE SERVICE";
            setColor(11);
            cout << " ----------" << endl;
            setColor(15);

            int delID;
            cout << "\nEnter Service ID to delete: ";
 
           while (!(cin >> delID))
           {
                setColor(12);
                cout << "\nInvalid ID! Please enter numbers only." << endl;
                setColor(15);
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "Press ENTER to try again...";
                cin.get();
                cout << "\nEnter Service ID to delete: ";
            }

            ServiceList* lists[] = {&flights, &buses, &trains, &events, &cinema, &hotels, &cars};
            string names[] = {"Flight", "Bus", "Train", "Event", "Cinema", "Hotel", "Car"};

            bool deleted = false;

    for (int i = 0; i < 7; i++)
    {
        Service* s = lists[i]->findService(delID);
        if (s)
        {
            if (s == lists[i]->head && s == lists[i]->tail)
            {
                delete s;
                lists[i]->head = lists[i]->tail = nullptr;
            }
            else
            {
                s->prev->next = s->next;
                s->next->prev = s->prev;
                if (s == lists[i]->head) lists[i]->head = s->next;
                if (s == lists[i]->tail) lists[i]->tail = s->prev;
                delete s;
            }

            setColor(10);
            cout << "\nSUCCESS: " << names[i] << " with ID " << delID << " has been DELETED!" << endl;
            setColor(7);
            deleted = true;
            
            if (deleted) {
    string sql = "DELETE FROM Services WHERE ServiceId = " + to_string(delID);
    execNonQuery(sql);
}
        }
    }

    if (!deleted)
    {
        setColor(12); 
        cout << "\nERROR: Service ID " << delID << " not found!" << endl;
        setColor(7);
    }

    cout << "\nPress ENTER to return to Admin Panel...";
    cin.ignore(10000, '\n');
    cin.get();
}

        else if (choice == 4)
{
    system("cls");
    setColor(11);
    cout << "------------------- ";
    setColor(15);
    cout << "ALL AVAILABLE SERVICES";
    setColor(11);
    cout << " -------------------\n\n";
    setColor(7);

    auto displayCategory = [](ServiceList* list, string title)
    {
        if (!list->head)
        {
            setColor(8);
            cout << "[ " << title << " ] - No services available.\n\n";
            setColor(7);
            return;
        }

        setColor(14);
        cout << "[ " << title << " ]" << endl;
        setColor(11);
        cout << string(title.length() + 4, '=') << endl;
        setColor(7);

        cout << left
             << setw(6)  << "ID"
             << setw(38) << "Service Name"
             << setw(18) << "From / Venue"
             << setw(16) << "To"
             << setw(8)  << "Seats"
             << "Price" << endl;
        setColor(8);
        cout << string(95, '-') << endl;
        setColor(15);

        Service* temp = list->head;
        do
        {
            string fromDisplay = temp->from;
            string toDisplay = temp->to;

            if (temp->name.length() > 35)
                temp->name = temp->name.substr(0, 32) + "...";

            if (fromDisplay.length() > 16)
                fromDisplay = fromDisplay.substr(0, 13) + "...";

            if (toDisplay.length() > 14)
                toDisplay = toDisplay.substr(0, 11) + "...";

            cout << left
                 << setw(6)  << temp->id
                 << setw(38) << temp->name
                 << setw(18) << fromDisplay
                 << setw(16) << toDisplay
                 << setw(8)  << temp->totalSeats
                 << "Rs." << fixed << setprecision(0) << temp->price << endl;

            temp = temp->next;
        } while (temp != list->head);

        cout << endl;
        setColor(7);
    };

    displayCategory(&flights, "FLIGHTS");
    displayCategory(&buses,   "BUSES");
    displayCategory(&trains,  "TRAINS");
    displayCategory(&events,  "EVENTS");
    displayCategory(&cinema,  "CINEMA");
    displayCategory(&hotels,  "HOTELS");
    displayCategory(&cars,    "CARS");

    setColor(11);
    cout << "-----------------------------------------------------------------------------------------------\n\n";
    setColor(15);
    cout << "Press ENTER to return to Admin Panel...";
    cin.get();
}

        else if (choice == 5)  
        {
            system("cls");
            setColor(11);
            cout << "---------------------- ";
            setColor(15);
            cout << "VENDORS LIST";
            setColor(11);
            cout << " ----------------------\n\n";
            setColor(14);
            cout << left << setw(12) << "Category" << setw(10) << "ID" << "Service Name" << endl;
            cout << string(58, '-') << endl;
            setColor(15);
            auto printVendor = [](ServiceList* list, string cat)
            {
                if (!list->head) return;
                Service* temp = list->head;
                do
                {
                    cout << left << setw(12) << cat << setw(10) << temp->id << temp->name << endl;
                    temp = temp->next;
                } while (temp != list->head);
            };

            printVendor(&flights, "Flight");
            printVendor(&buses, "Bus");
            printVendor(&trains, "Train");
            printVendor(&events, "Event");
            printVendor(&cinema, "Cinema");
            printVendor(&hotels, "Hotel");
            printVendor(&cars, "Car");

            cout << "\nPress ENTER to go back...";
            cin.get();
        }

        else if (choice == 6)
        {
            system("cls");
            users.showAllUsers();
        }
        
        else if (choice == 7)
{
    system("cls");

    setColor(11);
    cout << "\n------------------------------------------------------------\n";
    setColor(15);
    cout << "                 REVENUE & PROFIT REPORT\n";
    setColor(11);
    cout << "------------------------------------------------------------\n\n";
    setColor(7);

    double total = totalRevenue;
    double vendor = vendorPayment;
    double profit = ourProfit;

    // If no bookings yet
    if (total == 0)
    {
        setColor(8);
        cout << "           No booking made yet Revenue:  Rs.0\n\n";
        setColor(7);
    }
    else
    {
        setColor(14);
        cout << right << setw(45) << "TOTAL MONEY RECEIVED FROM USERS  : ";
        setColor(10);
        cout << "Rs. " << fixed << setprecision(2) << total << endl;

        setColor(14);
        cout << right << setw(45) << "TOTAL PAID TO VENDORS           : ";
        setColor(12);
        cout << "Rs. " << vendor << endl;

        setColor(11);
        cout << string(60, '-') << endl;

        setColor(14);
        cout << right << setw(45) << "YOUR PROFIT (15% COMMISSION)    : ";
        setColor(10);
        cout << "Rs. " << profit << endl;

        setColor(11);
        cout << string(60, '-') << endl << endl;
        setColor(7);
    }

    setColor(11);
    setColor(15);
    cout << "Press ENTER to return to Admin Panel...";
    cin.get();
}

else if (choice == 8) 
{
    system("cls");
    setColor(11);
    cout << "\n-------------------------------------------------------------\n";
    setColor(15);
    cout << "           ALL BOOKINGS IN SYSTEM (ADMIN VIEW)\n";
    setColor(11);
    cout << "-------------------------------------------------------------\n\n";
    setColor(7);

    bool hasAnyBooking = false;

    auto printBookings = [&](const char* icon, const char* category, int minID, int maxID)
    {
        bool found = false;
        User* u = users.head;
        if (!u) return;

        do
        {
            Booking* b = u->history.top;
            while (b)
            {
                if (b->serviceID >= minID && b->serviceID < maxID)
                {
                    if (!found)
                    {
                        setColor(14);
                        cout << icon << " " << category << " BOOKINGS" << endl;
                        setColor(11);
                        cout << string(strlen(category) + 19, '-') << endl;
                        setColor(7);
                        cout << left
                             << setw(12) << "User ID"
                             << setw(20) << "Customer Name"
                             << setw(12) << "Booking ID"
                             << setw(35) << "Service Booked"
                             << setw(8)  << "Seats"
                             << "Revenue" << endl;
                        setColor(8);
                        cout << string(100, '-') << endl;
                        setColor(15);
                        found = true;
                        hasAnyBooking = true;
                    }

                    cout << left
                         << setw(12) << u->id
                         << setw(20) << u->name
                         << setw(12) << b->bookingID
                         << setw(35) << b->serviceName
                         << setw(8)  << 1 
                         << "Rs." << fixed << setprecision(0) << b->paidAmount << endl;
                }
                b = b->next;
            }
            u = u->next;
        } while (u != users.head);

        if (found) cout << endl;
    };

    printBookings("Airplane", "FLIGHTS", 100, 200);
    printBookings("Bus",     "BUSES",   200, 300);
    printBookings("Train",    "TRAINS",   300, 400);
    printBookings("Party",    "EVENTS",   400, 500);
    printBookings("Movie",    "CINEMA",   500, 600);
    printBookings("Hotel",    "HOTELS",   600, 700);
    printBookings("Car",      "CARS",     700, 800);

    if (!hasAnyBooking)
    {
        setColor(8);
        cout << "No bookings have been made yet.\n\n";
        setColor(7);
    }

    double totalRev = totalRevenue;
    setColor(11);
    cout << "-------------------------------------------------------------\n";
    setColor(14);
    cout << right << setw(50) << "TOTAL SYSTEM REVENUE: ";
    setColor(10);
    cout << "Rs. " << fixed << setprecision(0) << totalRev << endl;

    setColor(14);
    cout << right << setw(50) << "YOUR PROFIT (15% COMMISSION): ";
    setColor(10);
    cout << "Rs. " << (totalRev * 0.15) << endl;

    setColor(14);
    cout << right << setw(50) << "VENDOR PAYABLE (85%): ";
    setColor(12);
    cout << "Rs. " << (totalRev * 0.85) << endl;

    setColor(11);
    cout << "-------------------------------------------------------------\n\n";
    setColor(15);
    cout << "Press ENTER to return to Admin Panel...";
    cin.get();
}
}
}


void start()
{
    int choice;
    string input;
    int id;

    while (true)
    {
    	system("cls");
    setColor(11);
    printSlow("===========", 1);
    setColor(15);
    printSlow(" BOOKING  MANAGEMENT  SYSTEM", 1);
    setColor(11);
    printSlow(" =============\n\n", 1);
    setColor(15);
    printSlow("1. Admin Login\n", 1);
    printSlow("2. Register\n", 1);
    printSlow("3. Login\n", 1);
    printSlow("0. Exit\n\n", 1);

    setColor(11);
    printSlow("Choice: ", 1);

        choice = -1;

        if (!(cin >> choice))
        {
            cout << "\nInvalid input! Please enter a number only." << endl;
            cin.clear();
            cin.ignore(10000, '\n');
            cout << "Press ENTER to continue...";
            cin.get();
            continue;
        }

        cin.ignore(10000, '\n');
        setColor(7);
        
        if (choice == 0)
        {
        	setColor(15);
            cout << "\nThank you for using Bookme.su!" << endl;
            cout << "Goodbye!\n";
            cout << "Press ENTER to exit...";
            cin.get();
            break;
        }

        else if (choice == 1)
        {
            adminMenu();
        }

        else if (choice == 2)
        {
            string name, cnic, phone;
            
            setColor(15);
            cout << "\nID: ";
            while (!(cin >> id))
            {
            	setColor(12);
                cout << "Invalid ID! Enter numbers only: ";
                setColor(15);
                cin.clear();
                cin.ignore(10000, '\n');
                cout << "\n\nID: ";
            }

            cout << "Name: ";
            cin.ignore();
            getline(cin, name);

            cout << "CNIC: ";
            getline(cin, cnic);

            cout << "Phone: ";
            getline(cin, phone);

            users.registerUser(id, name, cnic, phone);
            setColor(10);
            cout << "\nRegistration successful!\n" << endl;
            setColor(15);
            cout << "Press ENTER to continue...";
            cin.get();
        }

        else if (choice == 3)
{
    string input;
    int id;
    
    setColor(15);
    cout << "\nEnter User ID: ";
    cin >> input;

    bool validInput = true;
    for (char c : input)
    {
        if (!isdigit(c))
        {
            validInput = false;
            break;
        }
    }

    if (!validInput || input.empty())
    {
        setColor(12);
        cout << "\nInvalid input! User ID must contain only numbers." << endl;
        setColor(15);
        cout << "Press ENTER to try again...";
        cin.ignore(10000, '\n');
        cin.get();
        continue;
    }

    id = stoi(input);

    if (users.findUser(id) == nullptr)
    {
        setColor(12);
        cout << "\nInvalid User ID!" << endl;
        cout << "User ID " << id << " is not registered." << endl;
        cout << "Please register first or check your ID." << endl;
        setColor(7);
        cout << "\nPress ENTER to continue...";
        cin.ignore(10000, '\n');
        cin.get();
        continue;
    }

    setColor(10);
    cout << "\nLogin Successful! Welcome back!" << endl;
    setColor(7);
    cout << "Press ENTER to continue...";
    cin.ignore(10000, '\n');
    cin.get();

    userMenu(id);
}

        else
        {
        	setColor(12);
            cout << "\nInvalid choice! Please enter 0, 1, 2 or 3 only." << endl;
            setColor(15);
            cout << "Press ENTER to continue...";
            cin.get();
        }
    }
}
};




int main()
{
    if (!connectDatabase()) {
        cout << "Could not connect to database. Press ENTER to exit...";
        cin.get();
        return 1;
    }

    // Build system and load all data (Users, Services, Bookings, WaitingList) from DB
    BookmeSystem system;
    system.loadFromDatabase();

    // Run your full application (menus, booking, admin, etc.)
    system.start();

    // Close DB connection on exit
    disconnectDatabase();
    return 0;
}

// Made by Muhammad Saad Akhtar.