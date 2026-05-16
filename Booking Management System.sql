CREATE DATABASE BookingManagementSystemm;
GO
USE BookingManagementSystemm;
GO

-- USERS
CREATE TABLE Users (
    UserId  INT         PRIMARY KEY,
    Name    NVARCHAR(100) NOT NULL,
    Cnic    NVARCHAR(30)  NOT NULL,
    Phone   NVARCHAR(30)  NOT NULL
);

-- SERVICES (Flights, Buses, Trains, Events, Cinema, Hotels, Cars)
CREATE TABLE Services (
    ServiceId   INT           PRIMARY KEY,
    Category    NVARCHAR(20)  NOT NULL,  -- 'Flight','Bus','Train','Event','Cinema','Hotel','Car'
    Name        NVARCHAR(100) NOT NULL,
    FromLoc     NVARCHAR(100) NOT NULL,
    ToLoc       NVARCHAR(100) NOT NULL,
    DateTime    NVARCHAR(30)  NOT NULL,  -- you can later change to DATETIME if you want
    TotalSeats  INT           NOT NULL,
    BookedSeats INT           NOT NULL DEFAULT 0,
    Price       DECIMAL(10,2) NOT NULL
);

-- BOOKINGS (user history)
CREATE TABLE Bookings (
    BookingId   INT           PRIMARY KEY,
    UserId      INT           NOT NULL FOREIGN KEY REFERENCES Users(UserId),
    ServiceId   INT           NOT NULL FOREIGN KEY REFERENCES Services(ServiceId),
    ServiceName NVARCHAR(100) NOT NULL,
    Status      NVARCHAR(20)  NOT NULL,  -- 'Confirmed' / 'Cancelled'
    PaidAmount  DECIMAL(10,2) NOT NULL
);

-- WAITING LIST
CREATE TABLE WaitingList (
    WaitingId INT IDENTITY PRIMARY KEY,
    UserId    INT           NOT NULL FOREIGN KEY REFERENCES Users(UserId),
    UserName  NVARCHAR(100) NOT NULL,
    ServiceId INT           NOT NULL FOREIGN KEY REFERENCES Services(ServiceId)
);


-- Pre-registered users
INSERT INTO Users (UserId, Name, Cnic, Phone) VALUES
(101, 'Saad Akhtar',   '42201-1234567-1', '03001234567'),
(102, 'Usman Pamper',  '42201-9876543-2', '03219876543'),
(103, 'Rehman Odd',    '35201-5556667-3', '03331234567');

-- FLIGHTS (Category = 'Flight')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(101, 'Flight', 'PK-308 LHR-KHI', 'Lahore',    'Karachi',   '2025-11-27 10:00', 150, 0,  8500),
(102, 'Flight', 'PK-309 ISB-DXB', 'Islamabad', 'Dubai',     '2025-11-28 14:30', 120, 0, 25000),
(103, 'Flight', 'ER-310 KHI-LHR', 'Karachi',   'Lahore',    '2025-11-27 18:00', 140, 0,  8200),
(104, 'Flight', 'PA-311 LHE-ISB', 'Lahore',    'Islamabad', '2025-11-26 09:15', 100, 0,  4500),
(105, 'Flight', 'PK-312 ISB-KHI', 'Islamabad', 'Karachi',   '2025-11-29 20:00', 130, 0,  9000);

-- BUSES (Category = 'Bus')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(201, 'Bus', 'Daewoo Express LHR-ISB', 'Lahore',    'Islamabad', '2025-11-26 08:00', 45, 0, 1800),
(202, 'Bus', 'Faisal Movers  LHR-KHI', 'Lahore',    'Karachi',   '2025-11-27 21:00', 40, 0, 3500),
(203, 'Bus', 'Skyways        ISB-LHR', 'Islamabad', 'Lahore',    '2025-11-26 12:30', 50, 0, 1700),
(204, 'Bus', 'Bilal Travels  KHI-LHR', 'Karachi',   'Lahore',    '2025-11-28 19:00', 42, 0, 3600),
(205, 'Bus', 'Niazi Express  MUL-LHR', 'Multan',    'Lahore',    '2025-11-26 10:00', 48, 0, 1400);

-- TRAINS (Category = 'Train')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(301, 'Train', 'Khyber Mail',      'Karachi',   'Peshawar',   '2025-11-27 20:00', 400, 0, 2500),
(302, 'Train', 'Tezgam Express',   'Karachi',   'Rawalpindi', '2025-11-26 17:00', 380, 0, 2800),
(303, 'Train', 'Karachi Express',  'Lahore',    'Karachi',    '2025-11-28 18:00', 350, 0, 2700),
(304, 'Train', 'Shalimar Express', 'Lahore',    'Karachi',    '2025-11-29 06:00', 420, 0, 2400),
(305, 'Train', 'Green Line',       'Islamabad', 'Karachi',    '2025-11-27 15:00', 300, 0, 4500);

-- EVENTS (Category = 'Event')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(401, 'Event', 'PSL Final 2025',            'Gaddafi Stadium',      'N/A', '2025-12-15 19:00', 50000, 0, 3000),
(402, 'Event', 'Atif Aslam Live Concert',   'Expo Center Lahore',   'N/A', '2025-12-20 20:00', 10000, 0, 5000),
(403, 'Event', 'Food Festival 2025',        'Lahore Fort',          'N/A', '2025-12-25 16:00', 15000, 0, 1500),
(404, 'Event', 'Comedy Night - Zakir Khan', 'Alhamra Hall',         'N/A', '2025-11-30 20:30', 3000,  0, 3500),
(405, 'Event', 'Pakistan vs India - ODI',   'National Stadium Khi', 'N/A', '2025-12-10 14:00', 45000, 0, 8000);

-- CINEMA (Category = 'Cinema')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(501, 'Cinema', 'Deadpool & Wolverine 7PM', 'Cinepax Lahore',   'N/A', '2025-11-26 19:00', 250, 0,  900),
(502, 'Cinema', 'Joker 2 - 9PM Show',       'Universal Cinema', 'N/A', '2025-11-26 21:00', 200, 0, 1000),
(503, 'Cinema', 'Animal - 3D 6PM',          'DHA Cinema',       'N/A', '2025-11-27 18:00', 180, 0, 1200),
(504, 'Cinema', 'Nayab - Premiere',         'Cinegold Bahria',  'N/A', '2025-11-28 20:30', 300, 0, 1500),
(505, 'Cinema', 'Inside Out 2 - 4PM Kids',  'Cinepax Packages', 'N/A', '2025-11-26 16:00', 220, 0,  700);

-- HOTELS (Category = 'Hotel')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(601, 'Hotel', 'Pearl Continental Lahore', 'Lahore',    'N/A', '2025-11-27', 120, 0, 15000),
(602, 'Hotel', 'Avari Hotel Lahore',       'Lahore',    'N/A', '2025-11-28', 100, 0, 13000),
(603, 'Hotel', 'Serena Hotel Islamabad',   'Islamabad', 'N/A', '2025-11-27', 150, 0, 18000),
(604, 'Hotel', 'PC Bhurban Resort',        'Murree',    'N/A', '2025-11-29', 80,  0, 22000),
(605, 'Hotel', 'Movenpick Karachi',        'Karachi',   'N/A', '2025-11-30', 110, 0, 16000);

-- CARS (Category = 'Car')
INSERT INTO Services (ServiceId, Category, Name, FromLoc, ToLoc, DateTime, TotalSeats, BookedSeats, Price) VALUES
(701, 'Car', 'Toyota Corolla 2024',  'Lahore Airport',    'Return', '2025-11-27', 4, 0,  4000),
(702, 'Car', 'Honda Civic 2023',     'Islamabad Airport', 'Return', '2025-11-28', 4, 0,  4500),
(703, 'Car', 'Suzuki Alto (Budget)', 'Karachi Saddar',    'Return', '2025-11-26', 4, 0,  2500),
(704, 'Car', 'Toyota Prado 4x4',     'Lahore DHA',        'Return', '2025-11-29', 6, 0, 12000),
(705, 'Car', 'KIA Sportage',         'Islamabad F-10',    'Return', '2025-11-27', 5, 0,  8000);



SELECT TOP 1 * FROM Users;


SELECT * FROM Users;
SELECT * FROM Services;
SELECT * FROM Bookings;
SELECT * FROM WaitingList;