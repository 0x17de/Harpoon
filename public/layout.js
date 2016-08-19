class Layout {
    constructor() {
        this.layout()
        window.onresize = ()=>this.layout();
    }
    layout() {
        var l = q('div[data-layout]');
        console.dir(l);
        l.each((e)=>{
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
                    var sizeRequests = children.attr('data-layout-size').map((attr)=>{
                        if (!attr) return null;
                        var size = attr.split(":", 2)
                        // apply clipping
                        if (size[0] === 'fixed') {
                            clipped[splitProp[1]] -= size[1];
                            dynamicCount -= 1;
                        }
                        return size;
                    }).map((attr)=>{
                        if (attr) return attr;
                        // set relative size
                        return ['dynamic', 1.0/dynamicCount];
                    });

                    // save back
                    children.each((c, i)=>{
                        c.attr('data-layout-size',
                               sizeRequests[i][0] + ":" + sizeRequests[i][1]);
                    });

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
                        console.log(extent, req[1], JSON.stringify(clipped), JSON.stringify(props));

                        pane.css(props);
                    });
                }
            }
        });
    }
}
