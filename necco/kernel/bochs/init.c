extern void _Ports_Init(), _ISR_Init(), _Blit_Init(),
	    _Buf_Init(), _Stack_Init();

void
Cleese_Initialize()
{
	_Ports_Init();
	_ISR_Init();
	_Blit_Init();
	_Buf_Init();
	_Stack_Init();
}

void
Cleese_Finalize()
{
}
