// SPDX-License-Identifier: MIT
pragma solidity ^0.8.8;

contract DiplomaRegistry {
    struct Date {
        uint256 day;
        uint256 month;
        uint256 year;
    }

    struct Student {
        uint256 studentId;
        string studentName;
        string studentMajor;
        Date birthDate;
    }

    struct Diploma {
        Student student;
        string university;
        string degreeType;
        Date graduationDate;
    }

    Diploma[] private diplomas;
    mapping(uint256 => Student) private students; // Mapping student IDs to student data
    address private owner;

    constructor() {
        owner = msg.sender; // Set contract creator as owner
    }

    modifier onlyOwner() {
        require(msg.sender == owner, "Only owner can perform this action");
        _;
    }

    // Events
    event StudentAdded(uint256 studentId, string studentName);
    event DiplomaAdded(uint256 studentId, string university);

    function addStudent(uint256 studentId, string memory studentName, string memory studentMajor, uint256 day, uint256 month, uint256 year) public onlyOwner {
        Date memory birthDate = Date(day, month, year);
        students[studentId] = Student(studentId, studentName, studentMajor, birthDate);
        emit StudentAdded(studentId, studentName);
    }

    function addDiploma(uint256 studentId, string memory university, string memory degreeType, uint256 gradDay, uint256 gradMonth, uint256 gradYear) public onlyOwner {
        require(bytes(students[studentId].studentName).length != 0, "Student does not exist");
        Date memory graduationDate = Date(gradDay, gradMonth, gradYear);
        Diploma memory newDiploma = Diploma(students[studentId], university, degreeType, graduationDate);
        diplomas.push(newDiploma);
        emit DiplomaAdded(studentId, university);
    }

    function getDiplomaCount() public view returns (uint256) {
        return diplomas.length;
    }

    // Retrieve a student's data
    function getStudent(uint256 studentId) public view returns (Student memory) {
        return students[studentId];
    }
}
