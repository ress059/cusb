












// Definitely need to save the state of the transfer since more than 8 bytes sent.
// 1. GET_DESCRIPTOR() received
// 2. Send 8 bytes.
// 3. GET_DESCRIPTOR() received
// 4. Send next
// ...repeat until all data sent.