CREATE TABLE contents (
    id UUID PRIMARY KEY DEFAULT gen_random_uuid(),
    content TEXT,
    file TEXT,
    file_size FLOAT
);