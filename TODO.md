Code:
------------------------------------
cusbd.h/.c - TODO
configuration.h/.c - TODO
interface.h/.c - DONE
endpoint.h/.c - TODO
event.h/.c - DONE
setup_packet.h/.c - DONE
string.h/.c - DONE


Misc
-------------------------------------
1. Just use ECU_RUNTIME_ASSERT() for now. User still defines
ecu_assert_handler().
2. Maybe have user-defined callbacks that execute when default state,
address state, configuration state, etc are entered?
3. Do not valid assert if public function of type is called. 
Public functions responsible for valid asserting. I.e. 
```C
foo(dev, event)
{
    // dont assert csubd_event_valid() since public event function called.
    cusbd_event_id(event);
}

bar(dev, event)
{
    // assert cusbd_event_valid() since public function not called.
    ECU_RUNTIME_ASSERT( (cusbd_event_valid(event)) );
    do_stuff(event);
}
```


Testing
-------------------------------------
1. Can pretty much just do cusbd_dispatch().
    - SET_CONFIGURATION() then GET_CONFIGURATION() to test getters.




1. Follow USB state machine more. I.e. Suspended state, attached state, powered state
instead of on state off state.
2. Separate structures for IN vs OUT endpoints
```C
// Host to device.
struct cusbd_endpoint_out
{
    struct cusbd_descriptor base;
    struct cusbd_endpoint_descriptor descriptor;

    // hsm_dispatch(data_rx_event)
};

struct cusbd_endpoint_in
{
    struct cusbd_descriptor base;
    struct cusbd_endpoint_descriptor descriptor;
    void (*send)(const void *data, size_t len, void *obj);
};

struct cusbd_endpoint_zero
{
    void (*send)(size_t ep_num, const void *data, size_t len, void *obj);
    void (*stall)(size_t ep_num, void *obj);
    void (*nak)(size_t ep_num, void *obj);
    void (*ack)(size_t ep_num, void *obj);
};

struct cusbd
{
    struct cusbd_endpoint_zero *ep0;
};


void cusbd_send_data()
{
    struct event = {&endpoint, data,...};
    cusbd_dispatch(event);
        // Endpoint state machine (active - stalled - halted states).
        // private header! only cusbd hsm has access to endpoint hsm to ensure run to completion.
        cusbd_endpoint_dispatch(event); 
}
```

Endpoint types
------------------------------
1. Control transfer - For enumeration
2. Interrupt
    IN
    - Host periodically polls endpoint IN.
    - If no data endpoint returns NAK.
    - Else endpoint returns data and issues ACK.

    OUT
    - If host wants to send data it issues OUT token.
    - If endpoint OUT is busy/has data it issues NAK.
    - Else it stores the data and issues ACK.

3. Isochronous
    - Continuous data transfer. I.e. audio stream.

4. Bulk
    - Large bursty transfers. I.e. printer job.


Rules violated
--------------------------------------
1. Rule 3 (loop bounds) - L2
All loops shall have a statically determinable upper-bound on the
maximum number of loop iterations. It shall be possible for a static
compliance checking tool to affirm the existence of the bound. An
exception is allowed for the use of a single non-terminating loop per task
or thread where requests are received and processed. Such a server loop
shall be annotated with the C comment: /* @non-terminating@ */.
[Power of Ten Rule 2]
    - Tree FOR_EACH() and list FOR_EACH() macros do not have fixed upper bounds.

2. Rule 6 (inter-process communication) - L2
An IPC mechanism should be used for all task communication. Callbacks
should be avoided. No task should directly execute code or access data
that belongs to a different task. All IPC messages shall be received at a
single point in a task.
    - Basically saying only IPC mechanisms (queues, etc) allowed for communication between tasks.
    - Currently using callbacks to communicate b/w library and USB hardware.

3. Rule 13 (limited scope)
Data objects shall be declared at the smallest possible level of scope. No
declaration in an inner scope shall hide a declaration in an outer scope.
[MISRA-C:2004 Rule 8.7, 8.10; Power of Ten Rule 6]
    - Smallest scope and cannot shadow outer variables. I.e.
```C
for (int i = 0; i < 10; i++)
{
    int i = 1; // CANNOT do this. Shadows variable.
}

static void foo()
{
    bool status = false;

    if (something)
    {
        // status should be declared here not outside.
        status = true;
        bar(status);
    }
}
```

4. Rule 18 - L3
In compound expressions with multiple sub-expressions the intended
order of evaluation shall be made explicit with parentheses. [cf. MISRA-
C:2004 Rule 12.2]
- Something to keep note of but this only applies if order of execution is not defined by standard. I.e.
```C
// OK. Standard mandates left to right order of operations
if (a && b && c)

// INVALID. 
```

5. Rule 19 - L3
The evaluation of a Boolean expression shall have no side effects.
[MISRA-C:2004 Rule 13.1]
- Pretty sure I'm following but something to keep not of. I.e.
```C
// Invalid: Here, the count++ operation is a side effect.
if (count++ > 10) { ... } 

// Valid: If x and y are variables that are not modified within 
// this expression, this is a valid Boolean expression without side effects.
if (x > 10 && y < 5) { ... } 
```

6. Rule 29 - L4
Non-constant pointers to functions should not be used.

7. Rule 30 (type conversion) - L4
Conversions shall not be performed between a pointer to a function and
any type other than an integral type. [MISRA-C:2004 Rule 11.1]
- I.e. function pointer can only be converted to uintptr_t, int, etc.
- ECU violates this rule with function pointer upcasting.


Syntax
=========================
1. Variables referencing descriptor data are in camel case (same standard as USB).
Functions referencing descriptor data are in snake case (standard of this codebase).
I.e.
```C
extern void get_wlength();

uint16_t wLength = descriptor->wLength;
```
