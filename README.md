# Library Management System

## **Project Description**

This project implements a Library Management System using Object-Oriented Programming in C++. It enables users to add, borrow, return and reserve books, as well as maintain a record of borrowers, ensuring efficient library operations and user accessibility. The system efficiently manages books and users, demonstrating key concepts such as inheritance, encapsulation, abstraction, and polymorphism.

## **Features**

* Add books with details (title, author, publisher, ISBN, year).
* Remove or update existing books.
* Borrow, return and reserve books with status updates.
* Enforce borrowing limits (Students: 3 books, Faculty: 5 books).
* Overdue fines for students; faculty has extended borrowing privileges.
* Manage user accounts and track borrowed books.
* Data persistence via file handling.

## **Constraints (Limitations of the system)**

* Student IDs must be unique 3-digit numbers starting with 1 (e.g., 101, 102).
* Faculty IDs must be unique 3-digit numbers starting with 2.
* Librarian IDs must be unique 3-digit numbers starting with 3.
* Book IDs must be unique 4-digit numbers.
* Books cannot have duplicate identification numbers.
* Users cannot be added without specifying their type (Student, Faculty, Librarian) and a maximum of 100 users of each type can be added into the system.
* Librarians cannot be removed from the system.
* There is no limit on the number of books a user can reserve.
* A particular book can be reserved by only one user at a time.
* Data is saved to file when exiting the system using the interactive menu only.

## **Compilation & Execution Instructions**

Compiling the Software:

* Open a terminal or command prompt and navigate to the directory containing the source file "LMS.cpp".
* Compile the code using a C++ compiler that supports C++14 or later. For example, if MSVC compiler is used: cl /std:c++14 /Zi /Fe:LibraryManagementSystem.exe LMS.cpp
* This command compiles the source code and produces an executable named "LibraryManagementSystem.exe".

Executing the Software:

* On Unix/Linux/MacOS: ./LibraryManagementSystem.exe
* On Windows: LibraryManagementSystem.exe

## **Classes and Responsibilities**

* **User (Base Class)**
  + *Represents an abstract base class for all library users.*
  + **Attributes:** indentificationNumber
  + **Responsibilities:**
    1. Borrow, return, and reserve books.
    2. Ensure derived classes implement serialization and deserialization.

* **Student (Derived from User)**
  + *Represents a student who can borrow, return, and reserve books with limits and fines.*
  + **Additional Attributes:** account
  + **Responsibilities:**
    1. Borrow books with a limit of 3 and pay overdue fines
    2. Return books and update their status
    3. Reserve books if unavailable
    4. Save and load student data using file handling

* **Faculty (Derived from User)**
  + *Represents a faculty member who can borrow, return, and reserve books with extended limits.*
  + **Additional Attributes:** account
  + **Responsibilities:**
    1. Borrow books with a limit of 5 and overdue restrictions
    2. Return books and update their status
    3. Reserve books if unavailable
    4. Save and load faculty data using file handling

* **Librarian**
  + *Represents a staff member responsible for managing books and users in the library.*
  + **Responsibilities:**
    1. Add, remove, and update books in the library
    2. Add and remove users from the library system
    3. Cannot borrow, return, or reserve books
    4. Save and load librarian data using file handling

* **Book**
  + *Represents a book in the library system.*
  + **Attributes:** identificationNumber, title, author, publisher, ISBN, year, status
  + **Responsibilities:**
    1. Store book details and status.
    2. Allow access to book information.
    3. Handle data serialization and deserialization**.**

* **Account**
  + *Manages borrowed books, fines, and borrowing limits.*
  + **Attributes:** numberOfBorrowedBooks, fine, borrowedBooks, trackBooks
  + **Responsibilities:**
    1. Track borrowed books and due dates.
    2. Calculate fines for overdue books.
    3. Manage borrowing and returning of books.

* **Library**
  + *Manages books, users, and reservations in the system.*
  + **Attributes:** books, users, reserved
  + **Responsibilities:**
    1. Add, remove, and update books and users
    2. Manage book reservations
    3. Retrieve book and user details
    4. Save and load library data using file handling

## **Usage Instructions**

* Launch the system and enter your identification number.
* Students and Faculty:
  + Borrow books (limit apply).
  + Return books and update availability.
  + Reserve books if currently unavailable.
* Librarians:
  + Add, remove, and update book records.
  + Manage users by adding or removing them.
* Exit the system when done, ensuring data is saved.
