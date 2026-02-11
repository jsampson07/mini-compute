## Wire Format
- <header, raw_payload>
- How to parse?
    - Just extract header and payload_len
    - Then read "payload_len" amt of bytes from raw_payload
- What is stored in the *header*?
    - type = <JOB_TYPE>
        - To extract the type of job that is being requested
    - sender_id = <ID>
        - To know which client this job corresponds to when sending status(CANCEL, GET_STATUS, JOB_RESULT)
    - request_id = <ID>
        - To uniquely identify a job when assigning to worker (scheduling) or when trying to find other worker to place job on (if worker dies) ==> any interaction needed to identify a request
    - payload_len = <payload_length>
        - Used to parse the raw_payload to the exact # of bytes

## Format of Payload
- JSON

## Idea of serialization and deserialization
- Serialize
    - Process headers + payload in plaintext ==> produce bytes
    - Build headers + raw payload
- Deserialize
    - Recovers headers + payload string from byte format
    - Reads headers + raw payload back INTO a "Message"