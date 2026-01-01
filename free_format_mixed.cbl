       IDENTIFICATION DIVISION.
       PROGRAM-ID. FIXED-START.
      *Fixed format comment
>>SOURCE FORMAT IS FREE
*> Now in free format
PROCEDURE DIVISION.
    DISPLAY "Free format code"
    STOP RUN.
>>SOURCE FORMAT IS FIXED
       DISPLAY "Back to fixed".
      *Fixed comment again
       STOP RUN.
