#pragma once

class c_stack_attach_process
{
public:

    c_stack_attach_process( )
    {
    }

    c_stack_attach_process( PEPROCESS pe_process )
    {
        attach( pe_process );
    }

    ~c_stack_attach_process( )
    {
        detach( );
    }

    void attach( PEPROCESS pe_process )
    {
        KeStackAttachProcess( pe_process, &_apc );
        _attached = true;
    }

    void detach( )
    {
        if ( _attached )
        {
            _attached = false;
            KeUnstackDetachProcess( &_apc );
        }
    }

private:
    bool                        _attached = false;
    KAPC_STATE                  _apc {};
};

class c_pe_process
{
public:
    c_pe_process( ULONG pid )
    {
        _status = PsLookupProcessByProcessId( ( HANDLE )pid, &_pe_process );
    }

    ~c_pe_process( )
    {
        if ( success( ) )
            ObDereferenceObject( _pe_process );
    }

    PEPROCESS get_pe_process( )
    {
        return _pe_process;
    }

    NTSTATUS status( ) const
    {
        return _status;
    }

    bool success( ) const
    {
        return NT_SUCCESS( _status );
    }

private:
    NTSTATUS                    _status = STATUS_SUCCESS;
    PEPROCESS                   _pe_process;
};