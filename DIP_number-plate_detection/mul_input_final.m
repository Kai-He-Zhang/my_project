clear
file = dir(fullfile('*.jpg'));
for i = 1:length(file)
    file_name{i} = file(i).name;
end
txt_name = "411086006.txt";
fp = fopen(txt_name, "w");
count_error = 0;
count_nice = 0;
for q = 1:length(file_name)
    clearvars -except file_name file q txt_name fp count_error count_nice
    disp(file_name{q});
    origin_img = imread(file_name{q});
    gray_img = rgb2gray(origin_img);
    p1_filter = [-2 0 2; -2 0 2; -2 0 2];
    img2 = imfilter(gray_img, p1_filter);
    p2_filter = [2 0 -2; 2 0 -2; 2 0 -2];
    img3 = imfilter(gray_img, p2_filter);
    img2 = img2 + img3;
    img = img2 - max(img2(:))*0.5;
    img = im2bw(img, graythresh(img));
    % figure, imshow(img), impixelinfo
    img = medfilt2(img);
    img = medfilt2(img);
    img = medfilt2(img);
    [r, c] = size(img);
    square = ones(10, 20);
    img = imclose(img, square);
    % figure, imshow(img), impixelinfo
    labeledImg = bwlabel(img);
    stats = regionprops(labeledImg, 'BoundingBox', 'Area', 'Centroid');
    minArea = 1000;  
    validRegions = stats([stats.Area] > minArea);
    
    j = 0;
    % figure, imshow(origin_img), impixelinfo
    % hold on;
    for i = 1:length(validRegions)
        boundingBox = validRegions(i).BoundingBox;
        ratio = boundingBox(3)/boundingBox(4);
        if(ratio > 1.5 && ratio < 6)
            j = j + 1;
            validboxes(j) = validRegions(i);
            % rectangle('Position', boundingBox, 'EdgeColor', 'g', 'LineWidth', 2);
        end
    end
    % hold off;
    
    bboxs_cell = cell(0, 0);
    max_length = 0;
    number = 0;
    valid_char.BoundingBox = [0 0 0 0];
    valid_char.Area = 0;
    valid_char.Centroid = 0;
    for i = 1:j
        number(i) = 0;
        valid_cent = uint64(validboxes(i).Centroid);
        valid_bbox = uint64(validboxes(i).BoundingBox);
        x1 = valid_bbox(2) - 20;
        x2 = valid_bbox(2) + uint64(valid_bbox(4)) + 20;
        y1 = valid_bbox(1) - 20;
        y2 = valid_bbox(1) + uint64(valid_bbox(3)) + 20;
        if x1 == 0
            x1 = 1;
            continue;
        end
        if y1 == 0
            y1 = 1;
            continue;
        end
        [r, c] = size(gray_img);
        if x2 > r
            x2 = r;
        end
        if y2 > c
            y2 = c;
        end
        
        seg_img = gray_img( x1: x2,  y1: y2);
        [r, c] = size(seg_img);
        blockSize = [r, c];
        thresh = @(block_struct) ~im2bw(block_struct.data, graythresh(block_struct.data));
        seg_img = (blockproc(seg_img, blockSize, thresh));
        seg_img = imopen(seg_img, ones(3, 3));
        % figure, imshow(seg_img)
        labeledImg = bwlabel(seg_img);
        sstats = regionprops(labeledImg, 'BoundingBox', 'Area', 'Centroid');
        minArea = 100;  
        sstats = sstats([sstats.Area] > minArea);    
        if(size([sstats.Area], 2) < 6)
            continue;
        end
    
        % figure;
        % imshow(seg_img), impixelinfo
        c = 0;
        % hold on;
        for x = 1:length(sstats)
            boundingBox = sstats(x).BoundingBox;
            ratio = boundingBox(4)/boundingBox(3);
            if(ratio > 1)
                c = c + 1;
                valid_char(c) = sstats(x);
                % rectangle('Position', boundingBox, 'EdgeColor', 'g', 'LineWidth', 2);
            end
        end 
        % hold off;
        
        c = 0;
        for x = 1:length(valid_char)
            bbox(x, :) = valid_char(x).BoundingBox(:);
        end
        tmp_bbox = sortrows(bbox, 4, "descend");
        [r, c] = size(bbox);
        if r == 1
            continue;
        end
        mid_value = tmp_bbox(round(length(bbox)/2), 4);
        tmp_bbox(:, 4) = tmp_bbox(:, 4) - mid_value;
        valid_bboxs = tmp_bbox(-10 <= tmp_bbox(:, 4) & tmp_bbox(:, 4) <= 10, :);
        valid_bboxs(:, 4) = valid_bboxs(:, 4) + mid_value;
    
        tmp_bbox = sortrows(valid_bboxs, 2, 'descend');
        mid2_value = tmp_bbox(round(length(valid_bboxs(1))/2), 2);
        tmp_bbox(:, 2) = tmp_bbox(:, 2) - mid2_value;
        valid_bboxs = tmp_bbox(-16 <= tmp_bbox(:, 2) & tmp_bbox(:, 2) <= 5, :);
        valid_bboxs(:, 2) = valid_bboxs(:, 2) + mid2_value;
        
        if length(valid_bboxs) == 6 || length(valid_bboxs) == 7
            number(i) = 1;        
        end
        
        if max_length < length(valid_bboxs)
            max_length = length(valid_bboxs);
        end
    
        for x = 1:length(valid_bboxs)
            bboxs_cell{i} = valid_bboxs;
            bboxs_xy_cell{i} = [x1, y1];
        end
    
        if number(i) == 1
            break;
        end
    end
    
    number_sum = sum(number == 1);
    if number_sum ~= 1
        disp(file_name{q} + " error") 
        count_error = count_error + 1;
    else
        disp(file_name{q} + " nice")
        count_nice = count_nice + 1;
    end
    
    fprintf(fp, '%s\n', file_name{q});
    fprintf(fp, '%d\n', max_length);
    
    for i = 1:size(bboxs_cell, 2)
       [l, ~] = size(bboxs_cell{i});
       if l == max_length
            valid_bboxs = bboxs_cell{i};
            valid_bboxs = sortrows(valid_bboxs, 1, 'ascend');
            xy = bboxs_xy_cell{i};
            %figure, imshow(origin_img), impixelinfo
            %hold on;
            for x = 1:length(valid_bboxs)
                valid_bboxs(x, 1) = valid_bboxs(x, 1) + xy(2) - 1;
                valid_bboxs(x, 2) = valid_bboxs(x, 2) + xy(1) - 1;
                %rectangle('Position', valid_bboxs(x, :), 'EdgeColor', 'g', 'LineWidth', 2);
                fprintf(fp, "%d %d %d %d\n", valid_bboxs(x, 1), valid_bboxs(x, 2), valid_bboxs(x, 3), valid_bboxs(x, 4));
            end
            %hold off;
       end
    end
end
disp("error =" + count_error)
disp("nice = " + count_nice)