;/*****************************************************************************
;Filename    : guest_cmx.s
;Author      : pry
;Date        : 09/02/2018
;Description : The guest library for Cortex-M3/4/7.
;*****************************************************************************/

;/* Begin Header *************************************************************/
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
RVM_User_Stack
User_Stack      SPACE   0x00000100
__initial_sp

RVM_Int_Stack
Int_Stack       SPACE   0x00000400
    
                ;The align is "(2^3)/8=1(Byte)." In fact it does not take effect.            
                AREA            RESET,CODE,READONLY,ALIGN=3
                
                THUMB
                REQUIRE8
                PRESERVE8
;/* End Header ***************************************************************/

;/* Begin Exports ************************************************************/
                ;The initial stack pointer position
                EXPORT          __initial_sp
                ;The user stack pointer position
                EXPORT          RVM_User_Stack
                ;The interrupt stack pointer position
                EXPORT          RVM_Int_Stack   
                ;The interrupt handling routine receive endpoint 
                EXPORT          RVM_Int_Rcv
                ;Get the MSB in the word
                EXPORT          RVM_MSB_Get  
                ;Yield the processor, trigger the PendSV
                EXPORT          RVM_Yield  
                ;The atomic AND operation
                EXPORT          RVM_Fetch_And
                ;The hypercall maker
                EXPORT          _RVM_Hypercall
                ;The entry of the RVM
                EXPORT          _RVM_Entry
                ;The kernel function calling stub 
                EXPORT          _RVM_Kern
;/* End Exports **************************************************************/


;/* Begin Imports ************************************************************/
                ;The RVM image header
                IMPORT          RVM_Img
                ;The real entry
                IMPORT          __main
                ;The context switch interrupt flag
                IMPORT          RVM_Ctxsw
;/* End Imports **************************************************************/

;/* Begin Global Variable ****************************************************/
                DCD             RVM_Img
;/* End Global Variable ******************************************************/

;/* Begin Function:RVM_Entry **************************************************
;Description    : The entry of the OS, when running in an virtualized environment.
;Input          : None.
;Output         : None.
;Return         : None.   
;Register Usage : None. 
;*****************************************************************************/
_RVM_Entry
                 LDR     R0, =__main
                 BX      R0
;/* End Function:_RMP_Entry **************************************************/

;/* Begin Function:RVM_MSB_Get ************************************************
;Description    : Get the MSB of the word.
;Input          : ptr_t Val - The value.
;Output         : None.
;Return         : ptr_t - The MSB position.   
;Register Usage : None. 
;*****************************************************************************/
RVM_MSB_Get
                CLZ      R1,R0
                MOV      R0,#31
                SUB      R0,R1
                BX       LR
;/* End Function:RVM_MSB_Get *************************************************/

;/* Begin Function:RVM_Fetch_And **********************************************
;Description : The fetch-and-logic-and atomic instruction. Logic AND the pointer
;              value with the operand, and return the value before logic AND.
;Input       : ptr_t* Ptr - The pointer to the data.
;              ptr_t Operand - The number to logic AND with the destination.
;Output      : ptr_t* Ptr - The pointer to the data.
;Return      : ptr_t - The value before the AND operation.
;*****************************************************************************/
RVM_Fetch_And
                PUSH    {R4}
RVM_Fetch_And_Fail
                LDREX   R2,[R0]             ; Load exclusive
                AND     R3,R2,R1            ; OR operation
                STREX   R4,R3,[R0]          ; Store exclusive
                CMP     R4,#0x00            ; See if we succeeded
                BNE     RVM_Fetch_And_Fail  ; Go back if we fail
                MOV     R0,R2
                POP     {R4}
                BX      LR
;/* End Function:RVM_Fetch_And ***********************************************/

;/* Begin Function:_RVM_Hypercall *********************************************
;Description    : The stub to make a hypercall. This is actually a RME system call(asnd).
;Input          : None.
;Output         : None.
;Return         : None.   
;Register Usage : None. 
;*****************************************************************************/
_RVM_Hypercall
                PUSH       {R4-R5}     ; Manual clobbering
                MOV        R4,#0x20000 ; Asynchronous send
                MOV        R5,#0       ; To capability number 0.
                
                SVC        #0x00       ; System call
                ISB                    ; Instruction barrier - wait for instruction to complete.
                
                POP        {R4-R5}     ; Manual recovering
                BX         LR          ; Return from the call
                
                B          .           ; Shouldn't reach here.
;/* End Function:_RVM_Hypercall **********************************************/

;/* Begin Function:RVM_Int_Rcv ************************************************
;Description : Cause the interrupt handler routine to wait on the interrupt endpoint.
;Input       : None.
;Output      : None.                                      
;*****************************************************************************/
RVM_Int_Rcv
                PUSH       {R4-R5}       ; Manual clobbering
                MOV        R4,#0x30000   ; Asynchronous receive
                MOV        R5,#1         ; From capability number 1.
                
                SVC        #0x00         ; System call
                ISB                      ; Instruction barrier - wait for instruction to complete.
                
                POP        {R4-R5}       ; Manual recovering
                BX         LR            ; Return from the call
                NOP
;/* End Function:RVM_Int_Rcv *************************************************/

;/* Begin Function:RVM_Yield **************************************************
;Description : Trigger a yield to another thread. This will actually send a interrupt
;              to the interrupt thread.
;Input       : None.
;Output      : None.                                      
;*****************************************************************************/
RVM_Yield
                PUSH       {R4-R5}       ; Manual clobbering
                LDR        R4,=RVM_Ctxsw ; Set the context switch flag 
                MOV        R5,#1         ; Flag & capability number
                STR        R5,[R4]
                MOV        R4,#0x20000   ; Asynchronous send
                
                SVC        #0x00         ; System call
                ISB                      ; Instruction barrier - wait for instruction to complete.
                
                POP        {R4-R5}       ; Manual recovering
                BX         LR            ; Return from the call
                NOP
;/* End Function:RVM_Yield **************************************************/

;/* Begin Function:_RVM_Kern *************************************************
;Description : Invoke customized kernel functions which is compiled into the RME kernel.
;Input       : cnt_t Cap_ID - The capability ID.
;              ptr_t Func_ID - The function ID.
;              ptr_t Param1 - The first parameter.
;              ptr_t Param2 - The sencond parameter.
;Output      : None.
;Return      : ret_t - If successful, 0; else -1.
******************************************************************************/
_RVM_Kern
                PUSH       {R4-R7}       ; Manual clobbering
                ADD        R4,R0,#2      ; Capability ID - plus 2 because the first two are occupied.
                MOVT       R4,#0x4       ; Asynchronous receive
                MOV        R5,R1         ; Parameters
                MOV        R6,R2
                MOV        R7,R3
                
                SVC        #0x00         ; System call
                ISB                      ; Instruction barrier - wait for instruction to complete.
                
                POP        {R4-R7}       ; Manual recovering
                BX         LR            ; Return from the call
                NOP
;/* End Function:_RVM_Kern ***************************************************/

                END
;/* End Of File **************************************************************/

;/* Copyright (C) Evo-Devo Instrum. All rights reserved **********************/
