var dragInfo = null;

class Layout {
    constructor() {
        this.handles = q('#layout-handles');
        this.layout()
        q(window).on('resize', ()=>this.layout());
    }
    layout(opt_bKeepHandles) {
        if (!opt_bKeepHandles)
            this.handles.children().remove();

        q('div[data-layout]').each((e)=>{
            var layout = e.attr('data-layout')[0];
            var children = e.children();

            if (layout === 'root') {
                e.addClass('layout-root');
                children.addClass('layout');
                children.css({top:0,left:0,width:'100vw',height:'100vh'});
            } else if (layout === 'scroll') {
                e.addClass('layout-scroll');
            } else {
                children.addClass('layout');
                var size = children.size();
                var bounds = e.bounds()[0];
                var clipped = e.bounds()[0];

                if (layout === 'hbox' || layout === 'vbox') {
                    var splitProp, constProp;
                    if (layout === 'hbox') {
                        splitProp = ['left', 'width'];
                        constProp = ['top', 'height'];
                    } else {
                        splitProp = ['top', 'height'];
                        constProp = ['left', 'width'];
                    }
                    var dynamicCount = size;
                    var sizeRequests = children.attr('data-layout-size').map((attrStr)=>{
                        return attrStr ? attrStr.split(":", 2) : null;
                    });
                    var fixedRequests = sizeRequests.filter((e)=>e&&e[0] === 'fixed');
                    var initialpxRequests = sizeRequests.filter((e)=>e&&e[0] === 'initialpx');
                    fixedRequests.forEach((attr)=>{
                        clipped[splitProp[1]] -= attr[1]; // apply clipping
                        dynamicCount -= 1;
                    });
                    var initClipped = {top:clipped.top,
                                       left:clipped.left,
                                       width:clipped.width,
                                       height:clipped.height};
                    initialpxRequests.forEach((attr)=>{
                        attr[0] = 'dynamic';
                        initClipped[splitProp[1]] -= attr[1];
                        attr[1] = attr[1]/clipped[splitProp[1]];
                        dynamicCount -= 1;
                    });
                    sizeRequests.forEach((attr, i, arr)=>{
                        if (attr) return;
                        var dyn = initClipped[splitProp[1]]/clipped[splitProp[1]];
                        arr[i] = ['dynamic', dyn/dynamicCount]; // set relative size
                    });

                    // save back
                    children.each((c, i)=>{
                        c.attr('data-layout-size',
                               sizeRequests[i][0] + ":" + sizeRequests[i][1]);
                    });

                    var positions = [0];
                    var position = 0;
                    // apply size to elements
                    children.each((pane, i)=>{
                        var req = sizeRequests[i];
                        var props = {};
                        props[constProp[0]] = "0px";
                        props[constProp[1]] = clipped[constProp[1]]+"px";

                        props[splitProp[0]] = position+"px";
                        var extent;
                        if (req[0] === 'fixed')
                            extent = req[1];
                        else
                            extent = clipped[splitProp[1]] * req[1];
                        props[splitProp[1]] = extent+"px";
                        position += +extent;
                        positions.push(position);

                        pane.css(props);
                    });

                    if (!opt_bKeepHandles) {
                        // create handles
                        children.each((pane, i)=>{
                            if (i == 0)
                                return;
                            if (sizeRequests[i-1][0] === 'fixed' || sizeRequests[i][0] === 'fixed')
                                return;
                            var offset = pane.screenOffset()[0];
                            var prop = {};
                            offset[splitProp[0]] -= 5;
                            prop.left = offset.left+'px';
                            prop.top = offset.top+'px';
                            prop[splitProp[1]] = '10px';
                            prop[constProp[1]] = offset[constProp[1]]+'px';
                            prop.cursor = splitProp[0] === 'left' ? 'ew-resize' : 'ns-resize';
                            var handle = q('<div>')
                                .addClass("handle", "noselect")
                                .css(prop);
                            handle.on('mousedown', ()=>{
                                dragInfo = {
                                    handle: handle,
                                    topOrLeft: splitProp[0],
                                    children: children,
                                    clippedWidth: clipped[splitProp[1]],
                                    sizeRequests: sizeRequests,
                                    start: positions[i],
                                    i: i+0
                                };
                            });
                            this.handles.add(handle);
                        });
                    }
                }
            }
        });
    }
}
q(window).on('mousemove', (event)=>{
    if (!dragInfo) return;
    var mousePosition = q.cursor[dragInfo.topOrLeft];
    var i = dragInfo.i;
    var start = dragInfo.start;
    var sizeRequests = dragInfo.sizeRequests;

    // move handle
    var prop = {};
    prop[dragInfo.topOrLeft] = mousePosition-5 +'px';
    dragInfo.handle.css(prop);

    var width = dragInfo.clippedWidth;
    var newRel1 = (width*sizeRequests[i-1][1] - start + mousePosition) / width;
    var newRel2 = (width*sizeRequests[i][1] + start - mousePosition) / width;

    // limit movement
    if (newRel1 <= 0 || newRel1 >= 1
        || newRel2 <= 0 || newRel2 >= 1) return;

    sizeRequests[i-1][1] = newRel1;
    sizeRequests[i][1] = newRel2;

    // save back
    dragInfo.children.each((c, i)=>{
        c.attr('data-layout-size',
               sizeRequests[i][0] + ":" + sizeRequests[i][1]);
    });

    dragInfo.start = mousePosition; // changes are relative
    layout.layout(true);
});
q(window).on('mouseup', ()=>{
    if (dragInfo) {
        dragInfo = null;
        layout.layout(); // recreate handles
    }
});
