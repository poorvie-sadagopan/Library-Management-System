#include <fstream>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <queue>
#include <set>
#include <map>
#include <ctime>
using namespace std;

enum Status {Available, Borrowed, Reserved};

int getIntegerInput(string request)
{
    string line;
    int number = -1;
    while (number < 0)
    {
        cout << request;
        getline(cin, line);
        try {
            size_t idx;
            number = stoi(line, &idx);
            if (idx != line.length()) {
                number = -1;
            }
        } catch (const exception&) {
            number = -1;
        }
    }
    return number;
}

string getValidStringInput(string request){
    string output = "";
    while (output.empty()){
        cout << request;
        getline(cin, output);
    }
    return output;
}

class Book
{
    private:
        int identificationNumber, year;
        string title, author, publisher, ISBN;
        Status status;
    public:
        Book() {
            identificationNumber = 0;
            title = "";
            author = "";
            publisher = "";
            ISBN = "";
            year = 0;
            status = Available;
        }    
        Book(int ID, string t, string a, string p, string i, int y) {
            identificationNumber = ID;
            title = t;
            author = a;
            publisher = p;
            ISBN = i;
            year = y;
            status = Available;
        }
        int getIdentificationNumber() {
            return identificationNumber;
        }
        string getTitle() {
            return title;
        }
        string getAuthor() {
            return author;
        }
        string getPublisher() {
            return publisher;
        }
        string getISBN() {
            return ISBN;
        }
        int getYear() {
            return year;
        }
        Status getStatus() {
            return status;
        }
        void setStatus(Status s) {
            status = s;
        }
        void serialize(ofstream &outFile) {
            outFile << identificationNumber << endl;
            outFile << title << endl;
            outFile << author << endl;
            outFile << publisher << endl;
            outFile << ISBN << endl;
            outFile << year << endl;
            switch(status) {
                case Available:
                    outFile << "Available" << endl;
                    break;
                case Borrowed:
                    outFile << "Borrowed" << endl;
                    break;
                case Reserved:
                    outFile << "Reserved" << endl;
                    break;
            }
        }
        void deserialize(ifstream &inFile) {
            string line;
            getline(inFile, line);
            identificationNumber = stoi(line);
            getline(inFile, title);
            getline(inFile, author);
            getline(inFile, publisher);
            getline(inFile, ISBN);
            getline(inFile, line);
            year = stoi(line);
            string statusString;
            getline(inFile, statusString);
            status = Available;
            if (statusString == "Available") {
                status = Available;
            }
            else if (statusString == "Borrowed") {
                status = Borrowed;
            }
            else if (statusString == "Reserved" ){
                status = Reserved;
            }
        }
};

class Account
{
    private:
        int numberOfBorrowedBooks, fine;
        set<int> borrowedBooks;
        map<int, time_t> trackBooks;
        void addToBorrowed(int identificationNumber, time_t time) {
            borrowedBooks.insert(identificationNumber);
            trackBooks[identificationNumber] = time;
            numberOfBorrowedBooks++;
        }
    public:
        Account() {
            numberOfBorrowedBooks = 0;
            fine = 0;
        }
        int getNumberOfBorrowedBooks() {
            return numberOfBorrowedBooks;
        }
        void addToBorrowed(int identificationNumber) {
            addToBorrowed(identificationNumber, time(0));
        }
        void removeFromBorrowed(int identificationNumber) {
            auto it = find(borrowedBooks.begin(), borrowedBooks.end(), identificationNumber);
            if (it != borrowedBooks.end()) {
                borrowedBooks.erase(it);
            }
            trackBooks.erase(identificationNumber);
            numberOfBorrowedBooks--;
        }
        int calculateFines(Book* book) {
            int daysBorrowed = difftime(time(0), trackBooks[book->getIdentificationNumber()]) / 86400;
            if (daysBorrowed > 15) {
                fine = fine + 10 * (daysBorrowed - 15);
            }
            return fine;
        }
        int getFines() {
            return fine;
        }
        void payFines() {
            fine = 0;
        }
        int checkMaximumOverdue() {
            int maximumOverdue = 0;
            for (auto it = trackBooks.begin(); it != trackBooks.end(); it++) {
                int overdue = difftime(time(0), it->second) / 86400;
                if (overdue > maximumOverdue) {
                    maximumOverdue = overdue;
                }
            }
            return maximumOverdue;
        }
        bool checkBook(int identificationNumber) {
            if (borrowedBooks.find(identificationNumber) != borrowedBooks.end()){
                return true;
            }
            return false;
        }
        void serialize(ofstream &outFile){
            outFile << fine << endl;
            outFile << borrowedBooks.size() << endl;
            for (const auto book : borrowedBooks){
                outFile << book << " " << trackBooks[book] << endl;
            }
        }
        void deserialize(ifstream &inFile){
            string line;
            getline(inFile, line);
            fine = stoi(line);
            getline(inFile, line);
            int bookCount = stoi(line);
            for(int i = 0; i < bookCount; i++){
                int bookId; 
                time_t time; 
                inFile >> bookId;
                inFile >> time;
                getline(inFile, line);
                addToBorrowed(bookId, time);
            }
        }
};

class Library;

class User
{
    protected:
        int identificationNumber;
    public:
        User(int ID) {
            identificationNumber = ID;
        }
        int getIdentificationNumber() {
            return identificationNumber;
        }
        virtual void borrowBook(Library*, Book*) = 0;
        virtual void returnBook(Library*, Book*) = 0;
        virtual void reserveBook(Library*, Book*) = 0;
        virtual void payFines() = 0;
        virtual void serialize(ofstream &outFile) {
            outFile << identificationNumber << endl;
        }
        virtual void deserialize(ifstream &inFile) {}
};


class Library
{
    private:
        int nextStudentIdentificationNumber, nextFacultyIdentificationNumber, nextLibrarianIdentificationNumber;
        set<int> bookIdentificationNumbers, userIdentificationNumbers;
        map <int, Book*> books;
        map <int, User*> users;
        map <Book*, User*> reserved;
        User* createUser(int identificationNumber);
    public:
        Library() {
            nextStudentIdentificationNumber = 101;
            nextFacultyIdentificationNumber = 201;
            nextLibrarianIdentificationNumber = 301;
        }
        void addBook(Book* book) {
            bookIdentificationNumbers.insert(book->getIdentificationNumber());
            books[book->getIdentificationNumber()] = book;
        }
        void addUser(User* user) {
            userIdentificationNumbers.insert(user->getIdentificationNumber());
            users[user->getIdentificationNumber()] = user;
            if (user->getIdentificationNumber() / 100 == 1) {
                nextStudentIdentificationNumber = max(nextStudentIdentificationNumber, user->getIdentificationNumber()) + 1;
            }
            else if (user->getIdentificationNumber() / 100 == 2) {
                nextFacultyIdentificationNumber = max(nextFacultyIdentificationNumber, user->getIdentificationNumber()) + 1;
            }
            else {
                nextLibrarianIdentificationNumber = max(nextLibrarianIdentificationNumber, user->getIdentificationNumber()) + 1;
            }
        }
        void addBook(int identificationNumber) {
            if (books.find(identificationNumber) == books.end()) {
                int year;
                string title, author, publisher, ISBN;
                title = getValidStringInput("Enter Title: ");
                author = getValidStringInput("Enter Author: ");
                publisher = getValidStringInput("Enter Publisher: ");
                ISBN = getValidStringInput("Enter ISBN: ");
                year = getIntegerInput("Enter Year of Publication: ");
                Book* book = new Book(identificationNumber, title, author, publisher, ISBN, year);
                addBook(book);
                cout << "Book has been added successfully." << endl;
            }
            else {
                cout << "This book already exists." << endl;
            }
        }
        void removeBook(int identificationNumber) {
            if (books.find(identificationNumber) != books.end()) {
                bookIdentificationNumbers.erase(identificationNumber);
                books.erase(identificationNumber);
                cout << "Book has been removed successfully." << endl;
            }
            else {
                cout << "This book deos not exist." << endl;
            }
        }
        void updateBook(int identificationNumber) {
            if (books.find(identificationNumber) != books.end()) {
                int year;
                string title, author, publisher, ISBN;
                title = getValidStringInput("Enter Title: ");
                author = getValidStringInput("Enter Author: ");
                publisher = getValidStringInput("Enter Publisher: ");
                ISBN = getValidStringInput("Enter ISBN: ");
                year = getIntegerInput("Enter Year of Publication: ");
                Book* book = new Book(identificationNumber, title, author, publisher, ISBN, year);
                books[identificationNumber] = book;
                cout << "Book has been updated successfully." << endl;
            }
            else {
                cout << "This book does not exist." << endl;
            }
        }
        void addUser(string userType) {
            int identficationNumber = 0;
            if(userType == "Student") {
                identficationNumber = nextStudentIdentificationNumber;
            }
            else if (userType == "Faculty") {
                identficationNumber = nextFacultyIdentificationNumber;
            }
            else if (userType == "Librarian") {
                identficationNumber = nextLibrarianIdentificationNumber;
            }
            else {
                cout << "Invalid user type." << endl;
                return;
            }
            User* user = createUser(identficationNumber);
            if (user != nullptr) {
                addUser(user);
                cout << "User has been added successfully." << endl;
            }
        }
        void removeUser(int identificationNumber) {
            if (identificationNumber / 100 == 3) {
                cout << "Librarians cannot be deleted." << endl;
            }
            else if (users.find(identificationNumber) != users.end()) {
                userIdentificationNumbers.erase(identificationNumber);
                users.erase(identificationNumber);
                cout << "User has been removed successfully." << endl;
            }
            else {
                cout << "This user does not exist." << endl;
            }
        }
        void addToReserved(Book* book, User* user) {
            reserved[book] = user;
        }
        void removeFromReserved(Book* book) {
            reserved.erase(book);
        }
        int getNumberOfBooks() {
            return books.size();
        }
        User* getUser(int identificationNumber) {
            return users[identificationNumber];
        }
        Book* getBook(int identificationNumber) {
            return books[identificationNumber];
        }
        bool getReserved(Book* book, User* user) {
            return (reserved[book] == user);
        }
        bool checkIfReserved(Book* book) {
            return (reserved.find(book) != reserved.end());
        }
        bool checkBookIdentificationNumber(int identificationNumber) {
            if (bookIdentificationNumbers.find(identificationNumber) != bookIdentificationNumbers.end()) {
                return true;
            }
            return false;
        }
        bool checkUserIdentificationNumber(int identificationNumber) {
            if (userIdentificationNumbers.find(identificationNumber) != userIdentificationNumbers.end()) {
                return true;
            }
            return false;
        }
        void printBooks() {
            for (int ID : bookIdentificationNumbers) {
                cout << ID << " : " << getBook(ID)->getTitle() << endl;
            }
        }
        void printUsers() {
            for (int ID : userIdentificationNumbers) {
                cout << ID << " : ";
                if (ID / 100 == 1) {
                    cout << "Student" << endl;
                }
                else if (ID / 100 == 2) {
                    cout << "Faculty" << endl;
                }
                else {
                    cout << "Librarian" << endl;
                }
            }
        }
        void serialize(ofstream &outFile) {
            if (!outFile.is_open()) {
                cerr << "Error: Failed to open file for writing." << endl;
                return;
            }
            size_t bookCount = books.size();
            outFile << bookCount << " books" << endl;
            for (const auto &pair : books) {
                pair.second->serialize(outFile);
            }
            size_t userCount = users.size();
            outFile << userCount << " users" << endl;
            for (const auto &pair : users) {
                pair.second->serialize(outFile);
            }
            size_t reservedCount = reserved.size();
            outFile << reservedCount << endl;
            for (auto &pair : reserved) {
                outFile << pair.first->getIdentificationNumber() << endl;
                outFile << pair.second->getIdentificationNumber() << endl;
            }
        }
        void deserialize(ifstream &inFile) {
            if (!inFile.is_open()) {
                cerr << "Error: Failed to open file for reading." << endl;;
                return;
            }
            size_t bookCount;
            inFile >> bookCount;
            string ignore;
            getline(inFile, ignore);
            for (size_t i = 0; i < bookCount; i++) {
                Book* book = new Book();
                book->deserialize(inFile);
                addBook(book);
            }
            size_t userCount;
            inFile >> userCount;
            getline(inFile, ignore);
            for (size_t i = 0; i < userCount; i++) {
                int identificationNumber;
                string line;
                getline(inFile,line);
                identificationNumber = stoi(line);
                User* user = nullptr;
                user = createUser(identificationNumber);
                if (user) {
                    user->deserialize(inFile);
                    addUser(user);
                }
            }
            size_t reservedCount;
            inFile >> reservedCount;
            getline(inFile, ignore);
            for (size_t i = 0; i < reservedCount; i++) {
                int identificationNumber;
                string line;
                getline(inFile,line);
                identificationNumber = stoi(line);
                Book* book = getBook(identificationNumber);
                getline(inFile,line);
                identificationNumber = stoi(line);
                User* user = getUser(identificationNumber);
                addToReserved(book, user);
            }
        }                
};

class Student : public User
{
    private:
        Account account;
    public:
        Student(int ID) : User(ID) {}
        void borrowBook(Library* library, Book* book) override {
            if (account.checkBook(book->getIdentificationNumber())) {
                cout << "Book has already been borrowed by you." << endl;
            }
            else if (account.getNumberOfBorrowedBooks() >= 3) {
                cout << "Borrow limit exceeded." << endl;
            }
            else if (account.getFines() != 0) {
                cout << "Please pay existing fines before borrowing any other book." << endl;
            }
            else {
                if (book->getStatus() == Available) {
                    book->setStatus(Borrowed);
                    account.addToBorrowed(book->getIdentificationNumber());
                    cout << "Book has been borrowed successfully. Happy reading!" << endl;
                }
                else if (book->getStatus() == Reserved && library->getReserved(book, this)) {
                    book->setStatus(Borrowed);
                    account.addToBorrowed(book->getIdentificationNumber());
                    library->removeFromReserved(book);
                    cout << "Book has been borrowed successfully. Happy reading!" << endl;
                }
                else {
                    cout << "This book is currently unavailable." << endl;
                }
            }
        }
        void returnBook(Library* library, Book* book) override {
            if (account.checkBook(book->getIdentificationNumber())) {
                if (library->checkIfReserved(book) == true) {
                    book->setStatus(Reserved);
                }
                else {
                    book->setStatus(Available);
                }
                cout << "Book has been returned successfully. Current fines are " << account.calculateFines(book) << "." << endl;
                account.removeFromBorrowed(book->getIdentificationNumber());
            }
            else {
                cout << "Book has not been borrowed." << endl;
            }
        }
        void reserveBook(Library* library, Book* book) override {
            if (library->checkIfReserved(book)) {
                cout << "This book cannot be reserved currently." << endl;
            }
            else {
                if (book->getStatus() == Available) {
                    book->setStatus(Reserved);
                }
                library->addToReserved(book, this);
                cout << "Book has been reserved." << endl;
            }
        }
        void payFines() override {
            account.payFines();
            cout << "Fines have been paid." << endl;
        }
        void serialize(ofstream &outFile) override {
            User::serialize(outFile);
            account.serialize(outFile);
        }
        void deserialize(ifstream &inFile) override {
            User::deserialize(inFile);
            account.deserialize(inFile);
        }
};

class Faculty : public User
{
    private:
        Account account;
    public:
        Faculty(int ID) : User(ID) {}
        void borrowBook(Library* library, Book* book) override {
            if (account.checkBook(book->getIdentificationNumber())) {
                cout << "Book has already been borrowed by you." << endl;
            }
            else if (account.getNumberOfBorrowedBooks() >= 5) {
                cout << "Borrow limit exceeded." << endl;
            }
            else if (account.checkMaximumOverdue() > 60) {
                cout << "Return books that have an overdue for more than 60 days." << endl;
            }
            else {
                if (book->getStatus() == Available) {
                    book->setStatus(Borrowed);
                    account.addToBorrowed(book->getIdentificationNumber());
                    cout << "Book has been borrowed successfully. Happy reading!" << endl;
                }
                else if (book->getStatus() == Reserved && library->getReserved(book, this)) {
                    book->setStatus(Borrowed);
                    account.addToBorrowed(book->getIdentificationNumber());
                    library->removeFromReserved(book);
                    cout << "Book has been borrowed successfully. Happy reading!" << endl;
                }
                else {
                    cout << "This book is currently unavailable." << endl;
                }
            }
        }
        void returnBook(Library* library, Book* book) override {
            if (account.checkBook(book->getIdentificationNumber())) {
                if (library->checkIfReserved(book) == true) {
                    book->setStatus(Reserved);
                }
                else {
                    book->setStatus(Available);
                }
                account.removeFromBorrowed(book->getIdentificationNumber());
                cout << "Book has been returned successfully." << endl;
            }
            else {
                cout << "Book has not been borrowed." << endl;
            }
        }
        void reserveBook(Library* library, Book* book) override {
            if (library->checkIfReserved(book)) {
                cout << "This book cannot be reserved currently." << endl;
            }
            else {
                if (book->getStatus() == Available) {
                    book->setStatus(Reserved);
                }
                library->addToReserved(book, this);
                cout << "Book has been reserved." << endl;
            }
        }
        void payFines() override {
            cout << "No fines present." << endl;
        }
        void serialize(ofstream &outFile) override {
            User::serialize(outFile);
            account.serialize(outFile);
        }
        void deserialize(ifstream &inFile) override {
            User::deserialize(inFile);
            account.deserialize(inFile);
        }
};

class Librarian : public User
{
    public:
        Librarian(int ID) : User(ID) {}
        void borrowBook(Library* library, Book* book) override {
            throw runtime_error("Librarians cannot borrow books.");
        }
        void returnBook(Library* library, Book* book) override {
            throw runtime_error("Librarians cannot return books.");
        }
        void reserveBook(Library* library, Book* book) override {
            throw runtime_error("Librarians cannot reserve books.");
        }
        void payFines() override {
            throw runtime_error("Librarians need not pay fines.");
        }
        void addBook(Library* library, int identificationNumber) {
            library->addBook(identificationNumber);
        }
        void removeBook(Library* library, int identificationNumber) {
            library->removeBook(identificationNumber);
        }
        void updateBook(Library* library, int identificationNumber) {
            library->updateBook(identificationNumber);
        }
        void addUser(Library* library, string user) {
            library->addUser(user);
        }
        void removeUser(Library* library, User* user) {
            library->removeUser(user->getIdentificationNumber());
        }
        void serialize(ofstream &outFile) override {
            User::serialize(outFile);
        }
        void deserialize(ifstream &inFile) override {
            User::deserialize(inFile);
        }
};

User* Library::createUser(int identificationNumber)
{
    User* user = nullptr;
    if (identificationNumber / 100 == 1) {
        user = new Student(identificationNumber);
    }
    else if (identificationNumber / 100 == 2) {
        user = new Faculty(identificationNumber);
    }
    else if (identificationNumber / 100 == 3) {
        user = new Librarian(identificationNumber);
    }
    return user;
}

void createLibrary(Library &library)
{
    Book* book1 = new Book(1001, "Crime and Punishment", "Fyodor Dostoevsky", "The Russian Messenger", "0123456789", 1866);
    Book* book2 = new Book(1002, "The Great Gatsby", "F. Scott Fitzgerald", "Scribner", "1234567890", 1925);
    Book* book3 = new Book(1003, "1984", "George Orwell", "Secker & Warburg", "2345678901", 1949);
    Book* book4 = new Book(1004, "To Kill a Mockingbird", "Harper Lee", "J.B. Lippincott & Co.", "3456789012", 1960);
    Book* book5 = new Book(1005, "The Catcher in the Rye", "J.D. Salinger", "Little, Brown and Company", "4567890123", 1951);
    Book* book6 = new Book(1006, "Pride and Prejudice", "Jane Austen", "T. Egerton", "5678901234", 1813);
    Book* book7 = new Book(1007, "Moby-Dick", "Herman Melville", "Harper & Brothers", "6789012345", 1851);
    Book* book8 = new Book(1008, "War and Peace", "Leo Tolstoy", "The Russian Messenger", "7890123456", 1869);
    Book* book9 = new Book(1009, "Ulysses", "James Joyce", "Sylvia Beach", "8901234567", 1922);
    Book* book10 = new Book(1010, "The Odyssey", "Homer", "Ancient Greece", "9012345678", 1888);

    library.addBook(book1);
    library.addBook(book2);
    library.addBook(book3);
    library.addBook(book4);
    library.addBook(book5);
    library.addBook(book6);
    library.addBook(book7);
    library.addBook(book8);
    library.addBook(book9);
    library.addBook(book10);

    Student* student1 = new Student(101);
    Student* student2 = new Student(102);
    Student* student3 = new Student(103);
    Student* student4 = new Student(104);
    Student* student5 = new Student(105);

    library.addUser(student1);
    library.addUser(student2);
    library.addUser(student3);
    library.addUser(student4);
    library.addUser(student5);

    Faculty* faculty1 = new Faculty(201);
    Faculty* faculty2 = new Faculty(202);
    Faculty* faculty3 = new Faculty(203);

    library.addUser(faculty1);
    library.addUser(faculty2);
    library.addUser(faculty3);

    Librarian* librarian = new Librarian(301);
    library.addUser(librarian);
}

int main() {
    Library library;
    string filename = "library_data.txt";
    ifstream inFile(filename);
    if (inFile) {
        library.deserialize(inFile);
    }
    else {
        createLibrary(library);
    }
    inFile.close();

    while(true) {
        int userIdentificationNumber;
        cout << "Current users present in the library:" << endl;
        library.printUsers();
        userIdentificationNumber = getIntegerInput("Enter Identification Number (to exit, enter 0): ");
        if (userIdentificationNumber == 0){
            break;
        }
        if (!library.checkUserIdentificationNumber(userIdentificationNumber)) {
            cout << "Invalid user identification number." << endl;
            continue;
        }
        while(true) {
            if (library.getNumberOfBooks() == 0) {
                cout << "No book available." << endl;
            }
            User* user = library.getUser(userIdentificationNumber);
            if (userIdentificationNumber / 100 == 1) {
                cout << "Possible Actions:" << endl;
                cout << "(1) Borrow Book\n(2) Return Book\n(3) Reserve Book\n(4) Pay Fines\n(5) Exit" << endl;
                int option, bookID;
                option = getIntegerInput("Enter Option: ");
                if (option == 5) {
                    break;
                }
                else if (option == 1 || option == 2 || option == 3) {
                    cout << "Current books present in the library:" << endl;
                    library.printBooks();
                    bookID = getIntegerInput("Enter Book Identification Number: ");
                    if (!library.checkBookIdentificationNumber(bookID)) {
                        cout << "Invalid book identification number." << endl;
                        continue;
                    }
                    switch (option) {
                        case 1:
                            user->borrowBook(&library, library.getBook(bookID));
                            break;
                        case 2:
                            user->returnBook(&library, library.getBook(bookID));
                            break;
                        case 3:
                            user->reserveBook(&library, library.getBook(bookID));
                    }
                }
                else if (option == 4) {
                    user->payFines();
                }
                else {
                    cout << "Invalid Action." << endl;
                }
            }
            else if (userIdentificationNumber / 100 == 2) {
                cout << "Possible Actions:" << endl;
                cout << "(1) Borrow Book\n(2) Return Book\n(3) Reserve Book\n(4) Exit" << endl;
                int option, bookID;
                option = getIntegerInput("Enter Option: ");
                if (option == 4) {
                    break;
                }
                else if (option == 1 || option == 2 || option == 3) {
                    cout << "Current books present in the library:" << endl;
                    library.printBooks();
                    bookID = getIntegerInput("Enter Book Identification Number: ");
                    if (!library.checkBookIdentificationNumber(bookID)) {
                        cout << "Invalid book identification number." << endl;
                        continue;
                    }
                    switch (option) {
                        case 1:
                            user->borrowBook(&library, library.getBook(bookID));
                            break;
                        case 2:
                            user->returnBook(&library, library.getBook(bookID));
                            break;
                        case 3:
                            user->reserveBook(&library, library.getBook(bookID));
                    }
                }
                else {
                    cout << "Invalid Action." << endl;
                }
            }
            else if (userIdentificationNumber / 100 == 3) {
                Librarian* librarian = dynamic_cast<Librarian*>(user);
                cout << "Possible Actions:" << endl;
                cout << "(1) Add Book\n(2) Remove Book\n(3) Update Book\n(4) Add User\n(5) Remove User\n(6) Exit" << endl;
                int option;
                option = getIntegerInput("Enter Option: ");
                if (option == 6) {
                    break;
                }
                if (option == 1) {
                    int bookID = -1;
                    cout << "Current books present in the library:" << endl;
                    library.printBooks();
                    while (bookID < 1000) {
                        bookID = getIntegerInput("Enter Book Identification Number: ");
                    }
                    librarian->addBook(&library, bookID);
                }
                else if (option == 2 || option == 3) {
                    int bookID;
                    cout << "Current books present in the library:" << endl;
                    library.printBooks();
                    bookID = getIntegerInput("Enter Book Identification Number: ");
                    if (!library.checkBookIdentificationNumber(bookID)) {
                        cout << "Invalid book identification number." << endl;
                        continue;
                    }
                    switch (option) {
                        case 2:
                            librarian->removeBook(&library, bookID);
                            break;
                        case 3:
                            librarian->updateBook(&library, bookID);
                            break;
                    }
                }
                else if (option == 4) {
                    string user;
                    cout << "Enter User Type (Student/Faculty/Librarian): ";
                    getline(cin, user);
                    librarian->addUser(&library, user);
                }
                else if (option == 5) {
                    int userID;
                    cout << "Current users present in the library:" << endl;
                    library.printUsers();
                    userID = getIntegerInput("Enter User Identification Number: ");
                    if (!library.checkUserIdentificationNumber(userID)) {
                        continue;
                    }
                    librarian->removeUser(&library, library.getUser(userID));
                }
                else {
                    cout << "Invalid Action." << endl;
                    break;
                }
            }
        }
    }
    ofstream outFile(filename);
    if (outFile) {
        library.serialize(outFile);
        cout << "Data saved successfully." << endl;
    }
    else {
        cerr << "Error: Could not save data." << endl;
    }
    outFile.close();
}