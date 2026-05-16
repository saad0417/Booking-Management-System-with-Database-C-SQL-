<div align="center">

# 📦 Booking Management System
### C++ · OOP · Data Structures · SQL Server (ODBC)

A console-based multi-service booking platform simulating real-world booking operations — flights, buses, trains, hotels, events, cinema, and car rentals — backed by a fully integrated SQL Server database.

---

</div>

## 🧩 Overview

Built entirely in **C++ with Object-Oriented Programming** and classic **Data Structures**, this system provides a complete booking lifecycle — from browsing and booking to cancellation and refunds — for both users and admins. All data is persisted via **SQL Server through ODBC**, ensuring real-time sync across sessions.

---

## ✨ Features

### 👤 User Panel
- Register & login with secure credentials
- Browse services across **7 categories**: Flights ✈️ · Buses 🚌 · Trains 🚆 · Events 🎤 · Cinema 🎬 · Hotels 🏨 · Cars 🚗
- Book services with seat selection
- Apply **promo codes** (`SAVE10`, `SAVE20`) for instant discounts
- Pay via **Card**, **JazzCash**, or **Easypaisa**
- View full booking history
- Cancel bookings with **75% refund policy**
- Join **waiting list** when seats are full
- **Smart search** by keyword + price filter

### 🛠️ Admin Panel
- Secure admin login
- Add / Edit / Delete services
- View all services, vendor-wise listings, and registered users
- **Revenue analytics**: total revenue, vendor payout (85%), platform profit (15%)
- Full system-wide booking report

---

## 🗄️ Database Schema (SQL Server via ODBC)

| Table | Columns |
|---|---|
| `Users` | UserId, Name, CNIC, Phone |
| `Services` | ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price |
| `Bookings` | BookingId, UserId, ServiceId, ServiceName, Status, PaidAmount |
| `WaitingList` | UserId, UserName, ServiceId |

**Supported operations:** `INSERT` · `UPDATE` · `DELETE` · `SELECT`

---

## 🧠 Core Concepts

| Area | Details |
|---|---|
| **OOP** | Classes & Objects, Inheritance, Polymorphism, Abstraction |
| **Data Structures** | Doubly & Circular Linked Lists, Queues (Waiting List), Stack (Booking History) |
| **Memory** | Dynamic memory allocation throughout |
| **DB Layer** | ODBC-connected SQL Server with full CRUD |

---

## 🏗️ System Architecture

```
BookmeSystem
│
├── User Module        → Registration, Login, Booking, History
├── Admin Module       → Service Management, Reports, Analytics
├── Service Module     → Flight, Bus, Train, Event, Cinema, Hotel, Car
└── Database Layer     → Users, Services, Bookings, WaitingList
```

---

## 💰 Revenue Model

```
User pays full amount
    ├── 85% → Vendor payout
    └── 15% → Platform commission

Cancellation: 75% refund to user, revenue adjusted dynamically
```

---

## ⚙️ Getting Started

### Requirements
- Visual Studio or Code::Blocks
- C++ Compiler (GCC / MSVC)
- SQL Server with ODBC Driver configured

### Steps
```bash
1. Clone the repository
2. Configure ODBC connection string in source
3. Import the provided SQL schema
4. Compile the project
5. Run the executable
```

---

## 🚀 Roadmap

- [ ] GUI version (Qt / WinForms)
- [ ] Online payment gateway integration
- [ ] Web version (MERN Stack)
- [ ] Mobile app
- [ ] Cloud database migration

---

## 👨‍💻 Developer

**Muhammad Saad Akhtar**
BS Software Engineering — Superior University Lahore
*Passionate about C++, OOP, and System Design*

---

<div align="center">

📜 *This project is for educational purposes. Free to use and modify.*

</div>