global process_switch

; void process_switch(u32int *old_esp, u32int new_esp);
; Saves the callee-saved registers of the current cooperative task and
; restores them from the next task's stack.
process_switch:
    push ebp
    push ebx
    push esi
    push edi

    mov eax, [esp + 20]       ; old_esp pointer (after 4 pushes)
    mov [eax], esp

    mov esp, [esp + 24]       ; new_esp argument was above saved frame

    pop edi
    pop esi
    pop ebx
    pop ebp
    ret
